---
aliases: [Homework 18 Solutions]
tags: [#homework/solutions, #course_hpc, #openmp]
sources: [HW18_Solution.md, Homework 18.pdf]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 18 Solutions

> [!abstract]
> Solutions to OpenMP programming exercises for Homework 18, covering parallel reductions, private variables, and Monte Carlo simulations.

## Problem 1: Parallel Maximum Finding

**Goal:** Find the largest value in an array of 1,000 random integers.

> [!code] Implementation
> ```c
> #pragma omp parallel for reduction(max:max_val)
> for (int i = 0; i < N; i++) {
>     if (arr[i] > max_val)
>         max_val = arr[i];
> }
> ```

**Why `reduction` is necessary:**
Without `reduction(max:max_val)`, multiple threads would attempt to write to `max_val` simultaneously, causing a race condition. The `reduction` clause provides each thread with a **private copy** (initialized to `INT_MIN`) and combines them safely at the end.

---

## Problem 2: Vector Addition

**Goal:** $C[i] = A[i] + B[i]$ using a temporary variable.

> [!code] Implementation
> ```c
> #pragma omp parallel for
> for (int i = 0; i < N; i++) {
>     double temp = A[i] + B[i];  // temp is private
>     C[i] = temp;
> }
> ```

**Why `temp` is safe:**
Because `temp` is declared **inside the loop body**, it is automatically private to each thread (stack-allocated per iteration). No synchronization is required as each element of `C` is written by exactly one thread.

---

## Problem 3: Monte Carlo Pi Estimation

**Goal:** Estimate $\pi$ using 1,000,000 iterations.

> [!code] Implementation
> ```c
> #pragma omp parallel firstprivate(seed) reduction(+:hits)
> {
>     seed += omp_get_thread_num() * 1000;
>     #pragma omp for
>     for (long i = 0; i < ITERATIONS; i++) {
>         double x = (double)rand_r(&seed) / RAND_MAX;
>         double y = (double)rand_r(&seed) / RAND_MAX;
>         if (x*x + y*y <= 1.0) hits++;
>     }
> }
> ```

**Key Clauses:**
- `firstprivate(seed)`: Each thread gets a copy of the base seed, then offsets it by its rank to ensure **independent random sequences**.
- `rand_r(&seed)`: **Thread-safe** random number generator.
- `reduction(+:hits)`: Safely accumulates local hit counts into the global total.

> [!equation] Pi Calculation
> $$\pi \approx 4 \times \frac{\text{hits inside circle}}{\text{total samples}}$$
