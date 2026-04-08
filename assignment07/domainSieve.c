#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Block Decomposition Macros
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_LOW((id)+1,p,n)-BLOCK_LOW(id,p,n))
#define BLOCK_OWNER(index,p,n) (((p)*(index)+1)-1)/(n)

#define CACHE_L1_SIZE 32768 // Typical 32KB L1 cache size

int main(int argc, char *argv[]) {
    int id, p;
    long long n, size, elements, i, j, k;
    long long low_value, high_value;
    char *marked;
    long long global_count = 0, local_count = 0;
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
    
    // Optimization 1: Eliminate Even Integers. Only represent odd numbers from 3 to n.
    // Total odd numbers to check = (n - 1) / 2
    elements = (n - 1) / 2;

    low_value = 3 + 2 * BLOCK_LOW(id, p, elements);
    high_value = 3 + 2 * BLOCK_HIGH(id, p, elements);
    size = BLOCK_SIZE(id, p, elements);

    marked = (char *)calloc(size, sizeof(char));
    if (marked == NULL) {
        printf("Process %d: Cannot allocate memory\n", id);
        MPI_Finalize();
        exit(1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    // Optimization 2: Eliminate Broadcasts. 
    // Each process independently finds primes up to sqrt(n)
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

    // Optimization 3: Reorganize Loops for Cache Efficiency (Block Sieving)
    // Iterate through our local array in chunks that fit comfortably in the L1 cache.
    long long block_size = CACHE_L1_SIZE; 
    long long num_blocks = (size + block_size - 1) / block_size;

    for (long long b = 0; b < num_blocks; b++) {
        long long block_start_idx = b * block_size;
        long long block_end_idx = block_start_idx + block_size - 1;
        if (block_end_idx >= size) block_end_idx = size - 1;

        long long block_low_val = low_value + 2 * block_start_idx;
        long long block_high_val = low_value + 2 * block_end_idx;

        // Iterate over all base primes for this specific cache-sized block
        for (i = 0; i <= sqrt_elements; i++) {
            if (!base_primes[i]) {
                long long prime = 3 + 2 * i;
                long long first;

                // Find the first multiple of the prime within this block
                if (prime * prime > block_low_val) {
                    first = (prime * prime - low_value) / 2;
                } else {
                    long long rem = block_low_val % prime;
                    if (rem == 0) {
                        first = block_start_idx;
                    } else {
                        // Math to align with odd multiples
                        long long multiple = block_low_val + (prime - rem);
                        if (multiple % 2 == 0) multiple += prime; 
                        first = (multiple - low_value) / 2;
                    }
                }

                // Sieve the current block
                for (j = first; j <= block_end_idx; j += prime) {
                    marked[j] = 1;
                }
            }
        }
    }

    // Count primes in local domain
    for (i = 0; i < size; i++) {
        if (!marked[i]) local_count++;
    }

    // Combine counts.
    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    elapsed_time += MPI_Wtime();

    if (!id) {
        // Add 1 to account for the only even prime, 2.
        printf("%lld primes are less than or equal to %lld\n", global_count + 1, n);
        printf("Total elapsed time: %10.6f seconds\n", elapsed_time);
    }

    free(marked);
    free(base_primes);
    MPI_Finalize();
    return 0;
}