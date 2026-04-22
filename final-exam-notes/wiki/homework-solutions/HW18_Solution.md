# Homework 18 — Solution
**CPSC 375: High-Performance Computing | Spring 2026**

---

## Problem 1

**Write a program that creates an array of 1,000 random integers and finds the largest value using a parallel loop.**

### Solution

The key is using a **`reduction(max:max_val)`** clause so each thread tracks a local maximum, and OpenMP automatically combines them into the global maximum at the end of the parallel region — avoiding any race condition on `max_val`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

#define N 1000

int main() {
    int arr[N];
    int max_val = INT_MIN;

    // Initialize array with random integers
    srand(42);
    for (int i = 0; i < N; i++)
        arr[i] = rand() % 100000;

    // Find the maximum in parallel using reduction
    #pragma omp parallel for reduction(max:max_val)
    for (int i = 0; i < N; i++) {
        if (arr[i] > max_val)
            max_val = arr[i];
    }

    printf("Largest value in array: %d\n", max_val);
    return 0;
}
```

**Compile and run:**
```bash
gcc -fopenmp -o hw18_1 hw18_1.c
./hw18_1
```

**Why `reduction(max:max_val)` is necessary:**  
Without it, multiple threads would read and write `max_val` concurrently — a race condition. The `reduction` clause gives each thread a **private copy** of `max_val` initialized to `INT_MIN`, then combines all private copies using the `max` operator into a single global result after the loop.

---

## Problem 2

**Write a program that creates three arrays A, B, C of size 10,000 and computes vector addition `C[i] = A[i] + B[i]`. Use a parallel loop where a temporary variable inside the loop holds the sum before assigning to C[i].**

### Solution

The `temp` variable is declared **inside** the parallel for loop body, so it is automatically **private** to each thread (stack-allocated per iteration). There is no data dependency between iterations, making this a perfectly parallel loop.

```c
#include <stdio.h>
#include <omp.h>

#define N 10000

int main() {
    double A[N], B[N], C[N];

    // Initialize arrays
    for (int i = 0; i < N; i++) {
        A[i] = (double)i;
        B[i] = (double)(i * 2);
    }

    // Parallel vector addition using a temporary variable
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        double temp = A[i] + B[i];  // temp is private (declared inside loop body)
        C[i] = temp;
    }

    // Verify a few results
    printf("C[0]    = %.1f  (expected %.1f)\n", C[0],    0.0 + 0.0);
    printf("C[1]    = %.1f  (expected %.1f)\n", C[1],    1.0 + 2.0);
    printf("C[9999] = %.1f  (expected %.1f)\n", C[9999], 9999.0 + 19998.0);

    return 0;
}
```

**Compile and run:**
```bash
gcc -fopenmp -o hw18_2 hw18_2.c
./hw18_2
```

**Why `temp` is safe:**  
Because `temp` is declared **inside** the loop body (not before the `#pragma omp parallel for`), each thread allocates its own copy of `temp` on its private stack. No synchronization is needed. Each element `C[i]` is written by exactly one thread (no two threads write to the same index), so there is no race condition on `C` either.

---

## Problem 3

**Write a program that estimates pi using Monte Carlo with 1,000,000 iterations. Use `firstprivate` for the random seed and `reduction` for the hit counter.**

### Solution

The Monte Carlo method estimates π by randomly sampling points in the unit square [0,1]×[0,1] and checking how many fall inside the unit circle (x² + y² ≤ 1). The ratio of hits to total samples approximates π/4.

```c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ITERATIONS 1000000

int main() {
    long hits = 0;
    int seed = 12345;  // Base seed — each thread will offset this

    /*
     * firstprivate(seed): each thread gets its own COPY of seed,
     *   initialized to the value of seed at the start of the region.
     *   We then offset it by the thread ID so each thread has a
     *   different starting seed, ensuring statistically independent
     *   random sequences.
     *
     * reduction(+:hits): each thread accumulates its own private
     *   hit count; OpenMP sums all private copies into the global
     *   `hits` at the end of the parallel region.
     */
    #pragma omp parallel firstprivate(seed) reduction(+:hits)
    {
        // Give each thread a unique seed offset
        seed += omp_get_thread_num() * 1000;

        #pragma omp for
        for (long i = 0; i < ITERATIONS; i++) {
            // Generate two random values in [0.0, 1.0]
            double x = (double)rand_r((unsigned int *)&seed) / RAND_MAX;
            double y = (double)rand_r((unsigned int *)&seed) / RAND_MAX;

            // Check if the point is inside the unit circle
            if (x * x + y * y <= 1.0)
                hits++;
        }
    }

    // π ≈ 4 * (hits inside circle / total samples)
    double pi_estimate = 4.0 * (double)hits / (double)ITERATIONS;
    printf("Estimated π = %.6f  (true π ≈ 3.141593)\n", pi_estimate);

    return 0;
}
```

**Compile and run:**
```bash
gcc -fopenmp -o hw18_3 hw18_3.c
./hw18_3
```

**Expected output (approximate):**
```
Estimated π = 3.141892  (true π ≈ 3.141593)
```

**Key design decisions:**

| Clause | Purpose |
|--------|---------|
| `firstprivate(seed)` | Each thread gets its own copy of `seed` initialized to the master value, then offset by `thread_num` so random sequences differ per thread |
| `rand_r(&seed)` | Thread-safe, re-entrant random number generator that uses the thread's private seed (avoids the thread-unsafe `rand()`) |
| `reduction(+:hits)` | Each thread accumulates local hits; OpenMP performs a parallel reduction (sum) at the barrier — no race condition on `hits` |
| `#pragma omp for` | Distributes the 1,000,000 iterations evenly across threads |
