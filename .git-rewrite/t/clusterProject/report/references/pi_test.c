#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    long count = 0;
    double x, y;

    // 1. Initialize the MPI environment FIRST
    MPI_Init(&argc, &argv);

    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // 2. STRONG SCALING: Total points remains fixed at 2.4 Billion.
    // We divide the workload evenly across however many cores are running.
    // (Adding 'L' ensures the C compiler treats this huge number as a long)
    long total_points = 2400000000L;
    long n_points = total_points / world_size; 

    // Start the timer
    double start_time = MPI_Wtime();

    // Use a unique seed for each rank based on time and rank ID
    // so they don't all generate the exact same random numbers!
    srand(time(NULL) + rank);

    // Monte Carlo loop
    for (long i = 0; i < n_points; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        
        if (x * x + y * y <= 1.0) {
            count++;
        }
    }

    // Sum all local 'count' variables into 'total_count' on Rank 0
    long total_count;
    MPI_Reduce(&count, &total_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Stop the timer
    double end_time = MPI_Wtime();

    // Only the head node (Rank 0) reports the final result
    if (rank == 0) {
        double pi = 4.0 * total_count / (double)total_points;
        printf("\n============================================\n");
        printf("       THREE IDIOTS CLUSTER REPORT          \n");
        printf("============================================\n");
        printf("Estimated Pi: %f\n", pi);
        printf("Total Cores:  %d\n", world_size);
        printf("Total Points: %ld\n", total_points);
        printf("Points/Core:  %ld\n", n_points);
        printf("Time Taken:   %.4f seconds\n", end_time - start_time);
        printf("============================================\n\n");
    }

    MPI_Finalize();
    return 0;
}
