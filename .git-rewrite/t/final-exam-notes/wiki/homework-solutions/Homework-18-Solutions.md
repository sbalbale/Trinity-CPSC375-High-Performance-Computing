---
aliases: [Homework 18 Solutions]
tags: [#homework/solutions, #course_hpc, #openmp]
sources: [Homework 18.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 18 Solutions

> [!abstract]
> Solutions to the practical OpenMP programming exercises for Homework 18, including parallel max reduction, vector addition, and Monte Carlo Pi estimation.

## Problem 1: Parallel Max Search

**Requirement:** Find the largest value in an array of 1,000 random integers using a parallel loop.

> [!code] Implementation
> ```c
> #include <omp.h>
> #include <stdio.h>
> #include <stdlib.h>
> #include <time.h>
> 
> int main() {
>     int n = 1000;
>     int a[n];
>     int max_val = -1;
> 
>     srand(time(NULL));
>     for (int i = 0; i < n; i++) a[i] = rand() % 10000;
> 
>     #pragma omp parallel for reduction(max:max_val)
>     for (int i = 0; i < n; i++) {
>         if (a[i] > max_val) max_val = a[i];
>     }
> 
>     printf("Max value: %d\n", max_val);
>     return 0;
> }
> ```

---

## Problem 2: Vector Addition

**Requirement:** Compute $C[i] = A[i] + B[i]$ for size 10,000 using a temporary variable inside the loop.

> [!code] Implementation
> ```c
> #include <omp.h>
> #include <stdio.h>
> 
> int main() {
>     int n = 10000;
>     double a[n], b[n], c[n];
> 
>     // Initialize arrays...
> 
>     #pragma omp parallel for
>     for (int i = 0; i < n; i++) {
>         // Temporary variable is private by default because 
>         // it is declared inside the parallel block
>         double temp = a[i] + b[i];
>         c[i] = temp;
>     }
> 
>     return 0;
> }
> ```

---

## Problem 3: Monte Carlo Pi Estimation

**Requirement:** Run 1,000,000 iterations. Use `firstprivate` for the seed and `reduction` for hits.

> [!code] Implementation
> ```c
> #include <omp.h>
> #include <stdio.h>
> #include <stdlib.h>
> 
> int main() {
>     long n = 1000000;
>     long hits = 0;
>     unsigned int seed = 12345; // Base seed
> 
>     #pragma omp parallel for firstprivate(seed) reduction(+:hits)
>     for (long i = 0; i < n; i++) {
>         // Ensure each thread has a unique sequence based on its ID
>         unsigned int local_seed = seed + omp_get_thread_num();
>         
>         double x = (double)rand_r(&local_seed) / RAND_MAX;
>         double y = (double)rand_r(&local_seed) / RAND_MAX;
> 
>         if (x*x + y*y <= 1.0) {
>             hits++;
>         }
>     }
> 
>     double pi_est = 4.0 * (double)hits / n;
>     printf("Estimated Pi: %f\n", pi_est);
>     return 0;
> }
> ```

### Key Explanations
1. **`reduction(max:max_val)`**: This ensures that each thread tracks its own local maximum and then performs a final comparison to find the global maximum at the end of the loop.
2. **Local Variables**: In Problem 2, `double temp` is declared inside the loop scope. In OpenMP, variables declared inside a parallel region are **private by default**.
3. **`firstprivate(seed)`**: This initializes each thread's local `seed` with the value of the master thread's `seed` (12345) upon entering the parallel region.
4. **`rand_r`**: Standard `rand()` is not thread-safe. `rand_r()` allows each thread to maintain its own state safely using a pointer to its local seed.
