#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Block decomposition helpers for domain decomposition mode.
#define BLOCK_LOW(id, p, n) ((id) * (n) / (p))
#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id) + 1, p, n) - 1)
#define BLOCK_SIZE(id, p, n) (BLOCK_LOW((id) + 1, p, n) - BLOCK_LOW(id, p, n))

#define CACHE_L1_SIZE 32768

static void run_domain_sieve(long long n, int id, int p) {
    long long size, elements;
    long long low_value, high_value;
    char *marked;
    long long global_count = 0;
    long long local_count = 0;
    double elapsed_time;

    // Optimization 1: represent only odd integers in [3, n].
    elements = (n - 1) / 2;

    low_value = 3 + 2 * BLOCK_LOW(id, p, elements);
    high_value = 3 + 2 * BLOCK_HIGH(id, p, elements);
    (void)high_value;
    size = BLOCK_SIZE(id, p, elements);

    marked = (char *)calloc(size, sizeof(char));
    if (marked == NULL) {
        printf("Process %d: Cannot allocate memory\n", id);
        MPI_Finalize();
        exit(1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    // Optimization 2: each process independently generates base primes <= sqrt(n).
    long long sqrt_n = (long long)sqrt((double)n);
    long long sqrt_elements = (sqrt_n - 1) / 2;
    char *base_primes = (char *)calloc(sqrt_elements + 1, sizeof(char));
    if (base_primes == NULL) {
        printf("Process %d: Cannot allocate base prime array\n", id);
        free(marked);
        MPI_Finalize();
        exit(1);
    }

    for (long long i = 0; i <= sqrt_elements; i++) {
        if (!base_primes[i]) {
            long long prime = 3 + 2 * i;
            for (long long j = i + prime; j <= sqrt_elements; j += prime) {
                base_primes[j] = 1;
            }
        }
    }

    // Optimization 3: sieve local domain in cache-sized blocks.
    long long block_size = CACHE_L1_SIZE;
    long long num_blocks = (size + block_size - 1) / block_size;

    for (long long b = 0; b < num_blocks; b++) {
        long long block_start_idx = b * block_size;
        long long block_end_idx = block_start_idx + block_size - 1;
        if (block_end_idx >= size) {
            block_end_idx = size - 1;
        }

        long long block_low_val = low_value + 2 * block_start_idx;

        for (long long i = 0; i <= sqrt_elements; i++) {
            if (!base_primes[i]) {
                long long prime = 3 + 2 * i;
                long long first;

                if (prime * prime > block_low_val) {
                    first = (prime * prime - low_value) / 2;
                } else {
                    long long rem = block_low_val % prime;
                    if (rem == 0) {
                        first = block_start_idx;
                    } else {
                        long long multiple = block_low_val + (prime - rem);
                        if (multiple % 2 == 0) {
                            multiple += prime;
                        }
                        first = (multiple - low_value) / 2;
                    }
                }

                for (long long j = first; j <= block_end_idx; j += prime) {
                    marked[j] = 1;
                }
            }
        }
    }

    for (long long i = 0; i < size; i++) {
        if (!marked[i]) {
            local_count++;
        }
    }

    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    elapsed_time += MPI_Wtime();

    if (id == 0) {
        printf("[domain] %lld primes are less than or equal to %lld\n", global_count + 1, n);
        printf("[domain] Total elapsed time: %10.6f seconds\n", elapsed_time);
    }

    free(marked);
    free(base_primes);
}

static void run_functional_sieve(long long n, int id, int p) {
    long long elements;
    char *local_marked;
    char *global_marked = NULL;
    long long global_count = 0;
    double elapsed_time;

    // Functional model stores a full odd-only representation per process.
    elements = (n - 1) / 2;
    local_marked = (char *)calloc(elements, sizeof(char));
    if (local_marked == NULL) {
        printf("Process %d: Cannot allocate local marked array\n", id);
        MPI_Finalize();
        exit(1);
    }

    if (id == 0) {
        global_marked = (char *)calloc(elements, sizeof(char));
        if (global_marked == NULL) {
            printf("Process %d: Cannot allocate global marked array\n", id);
            free(local_marked);
            MPI_Finalize();
            exit(1);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    long long sqrt_n = (long long)sqrt((double)n);
    long long sqrt_elements = (sqrt_n - 1) / 2;
    char *base_primes = (char *)calloc(sqrt_elements + 1, sizeof(char));
    if (base_primes == NULL) {
        printf("Process %d: Cannot allocate base prime array\n", id);
        free(local_marked);
        if (global_marked != NULL) {
            free(global_marked);
        }
        MPI_Finalize();
        exit(1);
    }

    for (long long i = 0; i <= sqrt_elements; i++) {
        if (!base_primes[i]) {
            long long prime = 3 + 2 * i;
            for (long long j = i + prime; j <= sqrt_elements; j += prime) {
                base_primes[j] = 1;
            }
        }
    }

    // Round-robin assignment of base primes across ranks.
    int prime_index = 0;
    for (long long i = 0; i <= sqrt_elements; i++) {
        if (!base_primes[i]) {
            long long prime = 3 + 2 * i;
            if (prime_index % p == id) {
                long long first_idx = (prime * prime - 3) / 2;
                for (long long j = first_idx; j < elements; j += prime) {
                    local_marked[j] = 1;
                }
            }
            prime_index++;
        }
    }

    MPI_Reduce(local_marked, global_marked, elements, MPI_CHAR, MPI_BOR, 0, MPI_COMM_WORLD);

    elapsed_time += MPI_Wtime();

    if (id == 0) {
        for (long long i = 0; i < elements; i++) {
            if (!global_marked[i]) {
                global_count++;
            }
        }
        printf("[functional] %lld primes are less than or equal to %lld\n", global_count + 1, n);
        printf("[functional] Total elapsed time: %10.6f seconds\n", elapsed_time);
        free(global_marked);
    }

    free(local_marked);
    free(base_primes);
}

int main(int argc, char *argv[]) {
    int id, p;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (argc != 3) {
        if (id == 0) {
            printf("Usage: %s <domain|functional> <n>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    const char *mode = argv[1];
    long long n = atoll(argv[2]);

    if (n < 2) {
        if (id == 0) {
            printf("0 primes are less than or equal to %lld\n", n);
        }
        MPI_Finalize();
        return 0;
    }

    if (strcmp(mode, "domain") == 0) {
        run_domain_sieve(n, id, p);
    } else if (strcmp(mode, "functional") == 0) {
        run_functional_sieve(n, id, p);
    } else {
        if (id == 0) {
            printf("Unknown mode '%s'. Use 'domain' or 'functional'.\n", mode);
        }
        MPI_Finalize();
        return 1;
    }

    MPI_Finalize();
    return 0;
}
