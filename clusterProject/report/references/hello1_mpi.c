#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor/node
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    // Print the custom message
    printf("Hello from the Three Idiots cluster! Rank %02d out of %02d running on node: %s\n", 
           world_rank, world_size, hostname);

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
