/*
 * File: nbody.c
 * Description:
 *   Parallel N-body gravitational simulation using MPI.
 *   Fully symmetric (peer-to-peer) decomposition:
 *     - All P ranks own a contiguous block of N/P particles.
 *     - Each rank computes all-pairs gravitational forces for its subset.
 *     - MPI_Allgatherv synchronizes updated positions after every timestep.
 *   Integration uses the leapfrog (Störmer-Verlet) scheme:
 *     half-kick → drift → forces → half-kick.
 * Author: Sean Balbale
 * Date: 4/26/2026
 *
 * Usage: ./nbody <N> <nsteps> <seed>
 *   N      : total number of particles
 *   nsteps : number of simulation timesteps to time
 *   seed   : RNG seed for particle initialization
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* ── Physical constants (normalized N-body units) ───────────────────────── */
#define G_CONST   1.0      /* gravitational constant                        */
#define SOFTENING 0.05     /* Plummer softening length ε (avoids r→0)      */
#define DT        0.01     /* leapfrog timestep                             */

/* ─────────────────────────────────────────────────────────────────────────
 * compute_forces()
 *
 * Computes softened gravitational forces on the locally-owned particles
 * [my_start, my_start + local_n) using all N global positions.
 *
 * Softened force law:
 *   F_ij = G * mj * (rj − ri) / (|rj − ri|² + ε²)^(3/2)
 *
 * Caches the denominator to minimize sqrts: inv_r3 = (r² + ε²)^{-3/2}
 * is computed once per pair and reused for all three force components.
 *
 * Complexity: O(local_n × N) per call = O(N²/P) per timestep.
 * ───────────────────────────────────────────────────────────────────────── */
static void compute_forces(
        int local_n, int my_start,
        const double * restrict px,
        const double * restrict py,
        const double * restrict pz,
        const double * restrict mass, int N,
        double * restrict fx,
        double * restrict fy,
        double * restrict fz)
{
    const double eps2 = SOFTENING * SOFTENING;

    for (int i = 0; i < local_n; i++) {
        const int    gi = my_start + i;
        const double xi = px[gi], yi = py[gi], zi = pz[gi];
        double fix = 0.0, fiy = 0.0, fiz = 0.0;

        for (int j = 0; j < N; j++) {
            if (j == gi) continue;          /* skip self-interaction        */
            double dx = px[j] - xi;
            double dy = py[j] - yi;
            double dz = pz[j] - zi;
            double r2 = dx*dx + dy*dy + dz*dz + eps2;
            double inv_r3 = 1.0 / (r2 * sqrt(r2));
            double mj_r3  = mass[j] * inv_r3;
            fix += mj_r3 * dx;
            fiy += mj_r3 * dy;
            fiz += mj_r3 * dz;
        }

        fx[i] = G_CONST * fix;
        fy[i] = G_CONST * fiy;
        fz[i] = G_CONST * fiz;
    }
}

/* ─────────────────────────────────────────────────────────────────────────
 * leapfrog_kick()
 *
 * Velocity half-step: v += (F/m) * (dt/2).
 * Called twice per full timestep (before and after drift).
 * ───────────────────────────────────────────────────────────────────────── */
static void leapfrog_kick(
        int local_n, int my_start,
        const double * restrict mass_all,
        const double * restrict fx,
        const double * restrict fy,
        const double * restrict fz,
        double * restrict vx,
        double * restrict vy,
        double * restrict vz,
        double dt)
{
    double half_dt = 0.5 * dt;
    for (int i = 0; i < local_n; i++) {
        double inv_m = 1.0 / mass_all[my_start + i];
        vx[i] += fx[i] * inv_m * half_dt;
        vy[i] += fy[i] * inv_m * half_dt;
        vz[i] += fz[i] * inv_m * half_dt;
    }
}

/* ─────────────────────────────────────────────────────────────────────────
 * leapfrog_drift()
 *
 * Position full-step: x += v * dt.
 * Updates only the locally-owned slice of the global position arrays
 * (px[my_start .. my_start+local_n-1]); MPI_Allgatherv will sync
 * the remaining elements afterward.
 * ───────────────────────────────────────────────────────────────────────── */
static void leapfrog_drift(
        int local_n, int my_start,
        double * restrict px,
        double * restrict py,
        double * restrict pz,
        const double * restrict vx,
        const double * restrict vy,
        const double * restrict vz,
        double dt)
{
    for (int i = 0; i < local_n; i++) {
        int gi = my_start + i;
        px[gi] += vx[i] * dt;
        py[gi] += vy[i] * dt;
        pz[gi] += vz[i] * dt;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * main()
 * ═══════════════════════════════════════════════════════════════════════════ */
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* ── Argument parsing ─────────────────────────────────────────────── */
    if (argc != 4) {
        if (rank == 0)
            fprintf(stderr,
                    "Usage: %s <N> <nsteps> <seed>\n"
                    "  N      : total number of particles\n"
                    "  nsteps : number of simulation timesteps\n"
                    "  seed   : RNG seed for initialization\n",
                    argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int N      = atoi(argv[1]);
    int nsteps = atoi(argv[2]);
    int seed   = atoi(argv[3]);

    if (N < nprocs) {
        if (rank == 0)
            fprintf(stderr,
                    "Error: N (%d) must be >= nprocs (%d)\n", N, nprocs);
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    if (N <= 0 || nsteps <= 0) {
        if (rank == 0)
            fprintf(stderr, "Error: N and nsteps must be > 0\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* ── Particle distribution ────────────────────────────────────────── */
    /* Distribute N particles as evenly as possible.                       */
    /* Ranks 0..(N%P − 1) each own one extra particle (rem pile).         */
    int *sendcounts = (int *)malloc(nprocs * sizeof(int));
    int *displs     = (int *)malloc(nprocs * sizeof(int));
    if (!sendcounts || !displs) { perror("malloc"); MPI_Abort(MPI_COMM_WORLD, 1); }

    int base_n = N / nprocs;
    int rem_n  = N % nprocs;
    displs[0]  = 0;
    for (int r = 0; r < nprocs; r++) {
        sendcounts[r] = base_n + (r < rem_n ? 1 : 0);
        if (r > 0) displs[r] = displs[r-1] + sendcounts[r-1];
    }

    int my_n     = sendcounts[rank];   /* number of particles I own        */
    int my_start = displs[rank];       /* global index of my first particle */

    /* ── Memory allocation ────────────────────────────────────────────── */
    /* Global position and mass arrays — replicated on every rank.         */
    double *px   = (double *)malloc(N * sizeof(double));
    double *py   = (double *)malloc(N * sizeof(double));
    double *pz   = (double *)malloc(N * sizeof(double));
    double *mass = (double *)malloc(N * sizeof(double));
    /* Local velocity and force arrays — sized to my_n.                    */
    double *vx   = (double *)calloc(my_n, sizeof(double));
    double *vy   = (double *)calloc(my_n, sizeof(double));
    double *vz   = (double *)calloc(my_n, sizeof(double));
    double *fx   = (double *)malloc(my_n * sizeof(double));
    double *fy   = (double *)malloc(my_n * sizeof(double));
    double *fz   = (double *)malloc(my_n * sizeof(double));

    if (!px || !py || !pz || !mass ||
        !vx || !vy || !vz || !fx || !fy || !fz) {
        perror("malloc");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* ── Initialization ───────────────────────────────────────────────── */
    /* Rank 0 generates positions and masses; broadcasts to all.           */
    if (rank == 0) {
        srand((unsigned)seed);
        for (int i = 0; i < N; i++) {
            /* Particles uniformly distributed in [-1, 1]³                  */
            px[i]   = (double)rand() / RAND_MAX * 2.0 - 1.0;
            py[i]   = (double)rand() / RAND_MAX * 2.0 - 1.0;
            pz[i]   = (double)rand() / RAND_MAX * 2.0 - 1.0;
            mass[i] = 1.0;   /* equal masses in N-body units               */
        }
    }
    MPI_Bcast(px,   N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(py,   N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(pz,   N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(mass, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Initialize local velocities with small random perturbations.        */
    srand((unsigned)(seed + rank * 100003));
    for (int i = 0; i < my_n; i++) {
        vx[i] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
        vy[i] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
        vz[i] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }

    /* ── Pre-loop force computation ───────────────────────────────────── */
    /* Leapfrog requires forces at t=0 before the first half-kick.         */
    compute_forces(my_n, my_start, px, py, pz, mass, N, fx, fy, fz);

    /* ── Simulation loop ──────────────────────────────────────────────── */
    /* Timing brackets the loop only; initialization is excluded.          */
    /* MPI_Barrier ensures all ranks start the clock simultaneously.        */
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    for (int step = 0; step < nsteps; step++) {

        /* Half-kick: v += (F/m) * dt/2 */
        leapfrog_kick(my_n, my_start, mass, fx, fy, fz, vx, vy, vz, DT);

        /* Drift: x += v * dt  (updates only local slice of px/py/pz)     */
        leapfrog_drift(my_n, my_start, px, py, pz, vx, vy, vz, DT);

        /* Synchronize global position arrays across all ranks.            */
        /* Each rank contributes its updated slice; all receive the full N. */
        MPI_Allgatherv(MPI_IN_PLACE, my_n, MPI_DOUBLE,
                       px, sendcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Allgatherv(MPI_IN_PLACE, my_n, MPI_DOUBLE,
                       py, sendcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Allgatherv(MPI_IN_PLACE, my_n, MPI_DOUBLE,
                       pz, sendcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);

        /* Recompute forces at updated positions. */
        compute_forces(my_n, my_start, px, py, pz, mass, N, fx, fy, fz);

        /* Second half-kick: completes the full leapfrog step. */
        leapfrog_kick(my_n, my_start, mass, fx, fy, fz, vx, vy, vz, DT);
    }

    double t_local = MPI_Wtime() - t_start;

    /* Reduce across all ranks: wall time = slowest rank (true bottleneck). */
    double t_wall;
    MPI_Reduce(&t_local, &t_wall, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("────────────────────────────────────────────────\n");
        printf("  Particles   : %d\n",    N);
        printf("  Timesteps   : %d\n",    nsteps);
        printf("  MPI procs   : %d\n",    nprocs);
        printf("  Wall time   : %.6f s\n", t_wall);
        printf("────────────────────────────────────────────────\n");
        fflush(stdout);
    }

    free(px); free(py); free(pz); free(mass);
    free(vx); free(vy); free(vz);
    free(fx); free(fy); free(fz);
    free(sendcounts); free(displs);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
