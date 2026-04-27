/*
 * File: mandelbrot.c
 * Description:
 *   Parallel Mandelbrot set renderer using MPI.
 *   Master-worker architecture:
 *     - Rank 0 is the master: it distributes row indices to workers and collects results.
 *     - Ranks > 0 are workers: they compute assigned rows and send results back to the master.
 * Author: Sean Balbale
 * Date: 4/26/2026
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/* ── Complex-plane viewport ─────────────────────────────────────────────── */
#define XMIN (-2.5)
#define XMAX (1.0)
#define YMIN (-1.25)
#define YMAX (1.25)

/* ── MPI tags ───────────────────────────────────────────────────────────── */
#define TAG_WORK 1   /* master → worker: one int (row index)             */
#define TAG_RESULT 2 /* worker → master: width+1 ints [row | iter...]   */
#define TAG_DONE 3   /* master → worker: shutdown signal                 */

/* ── Pixel type ─────────────────────────────────────────────────────────── */
typedef struct
{
    uint8_t r, g, b;
} RGB;

/* ─────────────────────────────────────────────────────────────────────────
 *  Color palette
 *  256-entry LUT built from a smooth polynomial curve in [0,1].
 *  Entry 0 is reserved for points inside the set (black).
 * ───────────────────────────────────────────────────────────────────────── */
static RGB palette[256];

static void build_palette(void)
{
    palette[0] = (RGB){0, 0, 0}; /* inside set → black          */
    for (int i = 1; i < 256; i++)
    {
        double t = (double)i / 255.0;
        /* Bernstein polynomial colormap — blue core → gold edge → white   */
        palette[i].r = (uint8_t)(fmin(1.0, 9.0 * (1 - t) * t * t * t) * 255);
        palette[i].g = (uint8_t)(fmin(1.0, 15.0 * (1 - t) * (1 - t) * t * t) * 255);
        palette[i].b = (uint8_t)(fmin(1.0, 8.5 * (1 - t) * (1 - t) * (1 - t) * t) * 255);
    }
}

static inline RGB colorize(int iters, int max_iter)
{
    if (iters == max_iter)
        return palette[0];             /* inside set               */
    return palette[1 + (iters % 255)]; /* cyclic palette lookup   */
}

/* ─────────────────────────────────────────────────────────────────────────
 *  escape()
 *  Returns the iteration count at which |z|² > 4, or max_iter if the
 *  orbit is bounded (point is inside the Mandelbrot set).
 *
 *  Uses the standard recurrence:  z_{n+1} = z_n² + c
 *  where c = (cr, ci) is the complex coordinate of the pixel.
 *  Squares are cached (zr2, zi2) to avoid recomputation in the expansion.
 * ───────────────────────────────────────────────────────────────────────── */
static inline int escape(double cr, double ci, int max_iter)
{
    double zr = 0.0, zi = 0.0;
    for (int n = 0; n < max_iter; n++)
    {
        double zr2 = zr * zr;
        double zi2 = zi * zi;
        if (zr2 + zi2 > 4.0)
            return n;
        zi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
    }
    return max_iter;
}

/* ─────────────────────────────────────────────────────────────────────────
 *  compute_row()
 *  Maps pixel column indices to complex-plane x-coordinates and evaluates
 *  escape() for each. Results are written into out[0..width-1].
 * ───────────────────────────────────────────────────────────────────────── */
static void compute_row(int row, int width, int height, int max_iter, int *out)
{
    double dy = (YMAX - YMIN) / (double)(height - 1);
    double dx = (XMAX - XMIN) / (double)(width - 1);
    double ci = YMAX - row * dy;

    for (int col = 0; col < width; col++)
    {
        double cr = XMIN + col * dx;
        out[col] = escape(cr, ci, max_iter);
    }
}

/* ─────────────────────────────────────────────────────────────────────────
 *  write_ppm()
 *  Writes a binary PPM (P6) image using POSIX stdio.
 *  The iteration array is linearized row-major: iters[row*width + col].
 * ───────────────────────────────────────────────────────────────────────── */
static void write_ppm(const char *path, int width, int height,
                      int max_iter, const int *iters)
{
    FILE *f = fopen(path, "wb");
    if (!f)
    {
        perror("fopen");
        return;
    }

    fprintf(f, "P6\n%d %d\n255\n", width, height);

    for (int i = 0; i < width * height; i++)
    {
        RGB c = colorize(iters[i], max_iter);
        fwrite(&c, sizeof(RGB), 1, f);
    }
    fclose(f);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  MASTER  (rank 0)
 * ═══════════════════════════════════════════════════════════════════════════
 *  Protocol:
 *    1. Seed each worker with its first row (TAG_WORK, payload = row index).
 *    2. Loop: blocking-receive any result (TAG_RESULT, payload = [row | iters]).
 *       - Store the completed row in the image buffer.
 *       - If rows remain, send the next row to the now-idle worker.
 *       - Otherwise, send TAG_DONE to retire the worker.
 *    3. After all rows are collected, write the PPM and report timing.
 *
 *  Message sizes:
 *    TAG_WORK   →  1 MPI_INT
 *    TAG_RESULT ← (width + 1) MPI_INT   [row_index | col_0 ... col_{W-1}]
 *    TAG_DONE   →  0 bytes
 * ═══════════════════════════════════════════════════════════════════════════ */
static void run_master(int nprocs, int width, int height,
                       int max_iter, const char *outpath)
{
    int *image = (int *)malloc((size_t)width * height * sizeof(int));
    int *rowbuf = (int *)malloc((size_t)(width + 1) * sizeof(int));
    if (!image || !rowbuf)
    {
        perror("malloc");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double t_start = MPI_Wtime();

    int next_row = 0;
    int pending = 0;

    /* ── Initial seeding: give each worker its first task ─────────────── */
    for (int w = 1; w < nprocs && next_row < height; w++, next_row++)
    {
        MPI_Send(&next_row, 1, MPI_INT, w, TAG_WORK, MPI_COMM_WORLD);
        pending++;
    }

    /* ── Dynamic scheduling loop ──────────────────────────────────────── */
    while (pending > 0)
    {
        MPI_Status status;
        MPI_Recv(rowbuf, width + 1, MPI_INT,
                 MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);
        pending--;

        int completed_row = rowbuf[0];
        memcpy(&image[(size_t)completed_row * width],
               &rowbuf[1],
               width * sizeof(int));

        int src = status.MPI_SOURCE;
        if (next_row < height)
        {
            MPI_Send(&next_row, 1, MPI_INT, src, TAG_WORK, MPI_COMM_WORLD);
            next_row++;
            pending++;
        }
        else
        {
            /* Retire idle worker */
            MPI_Send(NULL, 0, MPI_INT, src, TAG_DONE, MPI_COMM_WORLD);
        }
    }

    double t_elapsed = MPI_Wtime() - t_start;

    printf("────────────────────────────────────────────────\n");
    printf("  Resolution  : %d x %d\n", width, height);
    printf("  Max iter    : %d\n", max_iter);
    printf("  MPI procs   : %d\n", nprocs);
    printf("  Wall time   : %.6f s\n", t_elapsed);
    printf("  Output      : %s\n", outpath);
    printf("────────────────────────────────────────────────\n");
    fflush(stdout);

    build_palette();
    write_ppm(outpath, width, height, max_iter, image);

    free(image);
    free(rowbuf);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  WORKER  (rank > 0)
 * ═══════════════════════════════════════════════════════════════════════════
 *  Loops: receive a row index (TAG_WORK) → compute → send back result
 *  (TAG_RESULT). Exits on TAG_DONE.
 *
 *  The result buffer layout is:
 *    rowbuf[0]         = row index  (so master knows where to store it)
 *    rowbuf[1..width]  = escape iteration counts
 * ═══════════════════════════════════════════════════════════════════════════ */
static void run_worker(int width, int height, int max_iter)
{
    int *rowbuf = (int *)malloc((size_t)(width + 1) * sizeof(int));
    if (!rowbuf)
    {
        perror("malloc");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (;;)
    {
        MPI_Status status;
        int row;
        MPI_Recv(&row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_DONE)
            break;

        rowbuf[0] = row;
        compute_row(row, width, height, max_iter, &rowbuf[1]);

        MPI_Send(rowbuf, width + 1, MPI_INT, 0, TAG_RESULT, MPI_COMM_WORLD);
    }

    free(rowbuf);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  main()
 * ═══════════════════════════════════════════════════════════════════════════ */
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* ── Argument parsing ─────────────────────────────────────────────── */
    if (argc != 5)
    {
        if (rank == 0)
            fprintf(stderr,
                    "Usage: %s <width> <height> <max_iter> <output.ppm>\n"
                    "  width/height : image resolution in pixels\n"
                    "  max_iter     : iteration depth (e.g. 1000–4000)\n"
                    "  output.ppm   : output file path\n",
                    argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int max_iter = atoi(argv[3]);
    const char *outpath = argv[4];

    /* ── Sanity checks ────────────────────────────────────────────────── */
    if (width <= 0 || height <= 0 || max_iter <= 0)
    {
        if (rank == 0)
            fprintf(stderr, "Error: width, height, and max_iter must be > 0\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    if (nprocs < 2)
    {
        if (rank == 0)
            fprintf(stderr,
                    "Error: requires at least 2 MPI processes "
                    "(1 master + 1 worker)\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    if (height < nprocs - 1 && rank == 0)
        fprintf(stderr,
                "Warning: more workers (%d) than rows (%d); "
                "some workers will be idle\n",
                nprocs - 1, height);

    /* ── Dispatch ─────────────────────────────────────────────────────── */
    if (rank == 0)
        run_master(nprocs, width, height, max_iter, outpath);
    else
        run_worker(width, height, max_iter);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
