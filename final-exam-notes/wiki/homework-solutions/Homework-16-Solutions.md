---
aliases: [Homework 16 Solutions]
tags: [#homework/solutions, #course_hpc, #openmp]
sources: [Homework 16.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 16 Solutions

> [!abstract]
> Solutions to the OpenMP scoping and synchronization exercises for Homework 16, featuring a detailed analysis of race conditions and a practical programming implementation.

## Problem 1: Code Segment Analysis

**Code:**
```c
int a = 10;
int b = 20;
#pragma omp parallel private(a)
{
    int c = 30;
    a = omp_get_thread_num();
    b = b + a;
}
```

### A. Shared Variables
> [!info]
> **Variable `b`** is shared across all threads. It was declared outside the parallel region and was not explicitly included in a `private` clause.

### B. Private Variables
> [!info]
> **Variables `a` and `c`** are private to each thread. 
> - `a` is private because it was explicitly declared in the `private(a)` clause.
> - `c` is private because it was declared **inside** the parallel region block.

### C. Race Condition and Final Value
**Scenario:** Run with 4 threads (IDs 0, 1, 2, 3).

**Final Value of `b`:**
The final value of `b` is **non-deterministic**. If there were no race condition, the value would be $20 + 0 + 1 + 2 + 3 = 26$. However, due to the race condition, the value will likely be between 20 and 25.

**Explanation:**
This code results in a [[race-condition]] because multiple threads are performing a **read-modify-write** operation on the shared variable `b` simultaneously.
1. Thread 1 and Thread 2 might both read the initial value of `b` (20).
2. Thread 1 calculates $20 + 1 = 21$ and prepares to write.
3. Thread 2 calculates $20 + 2 = 22$ and prepares to write.
4. If Thread 2 writes its result after Thread 1, the update from Thread 1 is **overwritten and lost**.

---

## Problem 2: OpenMP Implementation

**Requirement:** Set threads to 8 and print thread information.

> [!code] C Code Snippet
> ```c
> #include <omp.h>
> #include <stdio.h>
> 
> int main() {
>     // A. Set the number of threads to 8
>     omp_set_num_threads(8);
> 
>     // B. Parallel directive
>     #pragma omp parallel
>     {
>         int id = omp_get_thread_num();
>         int total = omp_get_num_threads();
>         
>         printf("Hello from thread %d out of %d threads.\n", id, total);
>     }
>     
>     return 0;
> }
> ```

**Key Functions Used:**
- `omp_set_num_threads(n)`: Sets the requested thread count for subsequent parallel regions.
- `omp_get_thread_num()`: Returns the unique ID (0 to $N-1$) of the calling thread.
- `omp_get_num_threads()`: Returns the total number of threads in the current active team.
