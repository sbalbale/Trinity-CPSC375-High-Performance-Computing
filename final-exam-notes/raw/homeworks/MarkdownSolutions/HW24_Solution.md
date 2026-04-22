# Homework 24 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1

**Problem:** Modify the parallel Sieve of Eratosthenes program from Lecture 24 to incorporate the first improvement: it should not set aside memory for even integers. Benchmark your program, comparing its performance with the original parallel sieve program.

---

### Background

The original parallel sieve allocates a boolean array `marked[]` of size `n/p` per process, with each index representing all integers in that process's range (including evens). The first optimization eliminates even numbers entirely, since 2 is the only even prime. This halves memory usage and reduces the number of iterations.

---

### Key Idea

Instead of indexing by actual integer value, we represent only **odd integers ≥ 3**. The mapping between array index `i` and actual integer value becomes:

```
value(i) = 2*i + 3      (index 0 → 3, index 1 → 5, index 2 → 7, ...)
index(v) = (v - 3) / 2  (value 3 → 0, value 5 → 1, value 7 → 2, ...)
```

The total number of odd integers in range [3, n] is `(n - 1) / 2`.

---

### Modified Program

```c
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    long long n = atoll(argv[1]);

    // Total odd numbers to consider: indices for 3, 5, 7, ..., n (if n is even, n-1)
    // odd_count = number of odd integers in [3, n]
    long long odd_count = (n - 1) / 2;  // values: 3,5,...,2*odd_count+1

    // Block-distribute odd_count indices across 'size' processes
    long long local_count = odd_count / size;
    long long remainder   = odd_count % size;
    long long local_start = rank * local_count + (rank < remainder ? rank : remainder);
    if (rank < remainder) local_count++;
    long long local_end = local_start + local_count - 1;  // inclusive index

    // Allocate local sieve array (0 = prime candidate, 1 = marked composite)
    char *marked = calloc(local_count, sizeof(char));
    if (!marked) { MPI_Abort(MPI_COMM_WORLD, 1); }

    double start_time = MPI_Wtime();

    long long sqrt_n = (long long)sqrt((double)n);

    // Sieve: iterate over odd primes p starting at 3
    for (long long p = 3; p <= sqrt_n; p += 2) {
        long long p_idx = (p - 3) / 2;  // index of p in the global odd array

        // Broadcast: check if p is not yet marked (it's prime)
        // Determine which process owns p_idx
        long long owner_count = odd_count / size + (p_idx < (odd_count % size) ? 1 : 0);
        // (Simplified: root process broadcasts whether p is prime)
        char p_is_prime;
        long long p_owner = p_idx / (odd_count / size + 1);
        if (p_idx < remainder * (odd_count / size + 1))
            p_owner = p_idx / (odd_count / size + 1);
        else
            p_owner = remainder + (p_idx - remainder * (odd_count / size + 1)) / (odd_count / size);

        if (rank == (int)p_owner) {
            long long local_idx = p_idx - local_start;
            p_is_prime = !marked[local_idx];
        }
        MPI_Bcast(&p_is_prime, 1, MPI_CHAR, (int)p_owner, MPI_COMM_WORLD);

        if (!p_is_prime) continue;

        // Mark all odd multiples of p in local range
        // Smallest odd multiple of p >= p^2 is p*p (if p is odd, p*p is odd)
        long long first_multiple = p * p;
        if (first_multiple % 2 == 0) first_multiple += p; // ensure odd
        if (first_multiple < 2 * local_start + 3)
            first_multiple = 2 * local_start + 3;

        // Align first_multiple to be a multiple of p
        if (first_multiple % p != 0) {
            first_multiple += p - (first_multiple % p);
            if (first_multiple % 2 == 0) first_multiple += p;
        }

        for (long long multiple = first_multiple;
             multiple <= 2 * local_end + 3;
             multiple += 2 * p) {
            long long idx = (multiple - 3) / 2 - local_start;
            if (idx >= 0 && idx < local_count)
                marked[idx] = 1;
        }
    }

    // Count local primes
    long long local_primes = 0;
    for (long long i = 0; i < local_count; i++) {
        if (!marked[i]) local_primes++;
    }

    long long total_primes;
    MPI_Reduce(&local_primes, &total_primes, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double elapsed = MPI_Wtime() - start_time;
    double max_elapsed;
    MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        total_primes++;  // count 2 itself
        printf("Primes up to %lld: %lld\n", n, total_primes);
        printf("Time: %.4f seconds\n", max_elapsed);
    }

    free(marked);
    MPI_Finalize();
    return 0;
}
```

---

### Performance Analysis

**Expected improvements over the original:**

| Metric | Original | Improved (no evens) |
|--------|----------|---------------------|
| Memory per process | ~n/(2p) bytes | ~n/(4p) bytes (half) |
| Outer loop iterations | ~√n / 2 (odd primes only) | ~√n / 2 (same) |
| Inner marking iterations | ~n / (2p) per prime | ~n / (4p) per prime (half) |
| Estimated speedup | 1× | ~1.5–2× |

The primary benefit is **halved memory footprint** (better cache behavior on large n) and **~2× fewer marking operations** since even multiples are never stored. For large n (e.g., n = 10^8), the improved version should demonstrate measurable speedup due to cache effects and reduced memory bandwidth requirements.

---

## Question 2

**Problem:** Modify the parallel Sieve to incorporate both improvements: no memory for even integers AND use the sequential sieve locally on each process to find all primes between 3 and ⌊√n⌋, eliminating the `MPI_Bcast`.

---

### Key Idea

Instead of broadcasting each prime p to all processes, each process independently runs a **local sequential sieve** on the range [3, ⌊√n⌋] at startup. Since this is a small range (at most √n values), it's fast and produces identical results on every process — eliminating communication entirely.

---

### Modified Approach

```c
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long n     = atoll(argv[1]);
    long long sqrt_n = (long long)sqrt((double)n);

    // ---- Step 1: Each process independently sieves [3, sqrt_n] ----
    // small_count = number of odd integers in [3, sqrt_n]
    long long small_count = (sqrt_n - 1) / 2;
    char *small_sieve = calloc(small_count, sizeof(char));

    for (long long p = 3; p * p <= sqrt_n; p += 2) {
        long long p_idx = (p - 3) / 2;
        if (small_sieve[p_idx]) continue;
        for (long long m = p * p; m <= sqrt_n; m += 2 * p) {
            small_sieve[(m - 3) / 2] = 1;
        }
    }
    // small_sieve[i] == 0 means (2i+3) is prime

    // ---- Step 2: Distribute [3, n] odd indices across processes ----
    long long odd_count   = (n - 1) / 2;
    long long local_count = odd_count / size + (rank < odd_count % size ? 1 : 0);
    long long local_start = rank * (odd_count / size) + (rank < odd_count % size ? rank : odd_count % size);
    long long local_end   = local_start + local_count - 1;

    char *marked = calloc(local_count, sizeof(char));

    double start_time = MPI_Wtime();

    // ---- Step 3: Use local small_sieve primes to mark composites ----
    for (long long pi = 0; pi < small_count; pi++) {
        if (small_sieve[pi]) continue;  // not prime
        long long p = 2 * pi + 3;

        // First odd multiple of p in local range >= p^2
        long long first = p * p;
        if (first < 2 * local_start + 3) {
            long long diff = (2 * local_start + 3) - first;
            first += ((diff + 2*p - 1) / (2*p)) * (2*p);
        }
        if (first % 2 == 0) first += p;

        for (long long m = first; m <= 2 * local_end + 3; m += 2 * p) {
            long long idx = (m - 3) / 2 - local_start;
            if (idx >= 0 && idx < local_count)
                marked[idx] = 1;
        }
    }

    double elapsed = MPI_Wtime() - start_time;

    long long local_primes = 0;
    for (long long i = 0; i < local_count; i++)
        if (!marked[i]) local_primes++;

    long long total_primes;
    MPI_Reduce(&local_primes, &total_primes, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double max_elapsed;
    MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        total_primes++;  // count 2
        printf("Primes up to %lld: %lld\n", n, total_primes);
        printf("Time (no Bcast): %.4f seconds\n", max_elapsed);
    }

    free(small_sieve);
    free(marked);
    MPI_Finalize();
    return 0;
}
```

### Performance Analysis

Eliminating `MPI_Bcast` removes a synchronization point inside the prime-marking loop that previously forced all processes to wait for the broadcast of each successive prime. With the local pre-sieve:

- **No communication** during the main marking phase
- Each process works entirely independently after setup
- Scales much better at high process counts where broadcast overhead grows

Expected speedup over single-Bcast version: noticeable on large n and many processes, since broadcast overhead scales as O(log p) per prime up to √n.
