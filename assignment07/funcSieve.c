#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int id, p;
    long long n, i, j;
    char *local_marked;
    char *global_marked = NULL;
    long long global_count = 0;
    double elapsed_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (argc != 2) {
        if (!id) printf("Usage: %s <n>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    n = atoll(argv[1]);
    
    // Each process allocates an array representing odd numbers up to n.
    // (This still halves the memory, but every process holds a full copy of the bounds).
    long long elements = (n - 1) / 2;
    local_marked = (char *)calloc(elements, sizeof(char));
    
    if (id == 0) {
        global_marked = (char *)calloc(elements, sizeof(char));
    }

    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    // Step 1: Every process finds base primes up to sqrt(n)
    long long sqrt_n = (long long)sqrt((double)n);
    long long sqrt_elements = (sqrt_n - 1) / 2;
    char *base_primes = (char *)calloc(sqrt_elements + 1, sizeof(char));
    
    for (i = 0; i <= sqrt_elements; i++) {
        if (!base_primes[i]) {
            long long prime = 3 + 2 * i;
            for (j = i + prime; j <= sqrt_elements; j += prime) {
                base_primes[j] = 1;
            }
        }
    }

    // Step 2: Functional Decomposition 
    // Distribute the primes to processes using Round-Robin
    int prime_index = 0;
    for (i = 0; i <= sqrt_elements; i++) {
        if (!base_primes[i]) {
            long long prime = 3 + 2 * i;
            
            // Only process 'id' handles this prime
            if (prime_index % p == id) {
                long long first_idx = (prime * prime - 3) / 2;
                // Sieve the entire full-sized local array for this prime
                for (j = first_idx; j < elements; j += prime) {
                    local_marked[j] = 1;
                }
            }
            prime_index++;
        }
    }

    // Step 3: Merge all distributed full-size arrays into Process 0 using Logical OR
    MPI_Reduce(local_marked, global_marked, elements, MPI_CHAR, MPI_BOR, 0, MPI_COMM_WORLD);

    elapsed_time += MPI_Wtime();

    // Step 4: Process 0 counts the final unmarked elements
    if (id == 0) {
        for (i = 0; i < elements; i++) {
            if (!global_marked[i]) global_count++;
        }
        printf("%lld primes are less than or equal to %lld\n", global_count + 1, n);
        printf("Total elapsed time: %10.6f seconds\n", elapsed_time);
        free(global_marked);
    }

    free(local_marked);
    free(base_primes);
    MPI_Finalize();
    return 0;
}