---
aliases: [Homework 16 Solutions]
tags: [#homework/solutions, #course_hpc, #openmp]
sources: [HW16_Solution.md, Homework 16.pdf]
created: 2026-04-20
updated: 2026-04-21
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
> **Variable `b`** is shared across all threads. It is declared outside the parallel region and is **not** listed in the `private()` clause.

### B. Private Variables
> [!info]
> - **Variable `a`** is explicitly declared `private(a)` in the `#pragma omp parallel` directive. Each thread gets its own uninitialized copy.
> - **Variable `c`** is declared **inside** the parallel block, making it automatically private to each thread (stack-allocated).

### C. Race Condition with 4 Threads
**Scenario:** Run with 4 threads (IDs 0, 1, 2, 3).

**Final Value of `b`:**
The final value of `b` is **indeterminate** (non-deterministic).

**Explanation:**
This code results in a [[race-condition]] because multiple threads perform an unsynchronized **read-modify-write** on the shared variable `b`.
> [!warning] Read-Modify-Write Interleaving
> 1. **Read**: Thread 0 and Thread 1 both read `b = 20`.
> 2. **Compute**: Thread 0 computes $20+0=20$; Thread 1 computes $20+1=21$.
> 3. **Write**: Thread 0 writes `20`; Thread 1 writes `21`.
> Whichever thread writes last "wins," erasing the other's update.

---

## Problem 2: OpenMP Implementation

**Requirement:** Set threads to 8 and print thread information.

> [!code] C Code Snippet
> ```c
> #include <stdio.h>
> #include <omp.h>
> 
> int main() {
>     // A. Set the number of threads to 8 using a library function
>     omp_set_num_threads(8);
> 
>     // B. Parallel directive; each thread prints its greeting
>     #pragma omp parallel
>     {
>         int id    = omp_get_thread_num();   // This thread's ID (0-7)
>         int total = omp_get_num_threads();  // Total threads (8)
>         int procs = omp_get_num_procs();    // Available processors
> 
>         printf("Hello from thread %d out of %d threads. "
>                "(%d processors available)\n", id, total, procs);
>     }
> 
>     return 0;
> }
> ```

**Key Functions:**
- `omp_set_num_threads(8)` — sets thread count for subsequent parallel regions.
- `omp_get_thread_num()` — returns the calling thread's unique ID.
- `omp_get_num_threads()` — returns the total number of threads in the team.
- `omp_get_num_procs()` — returns physical hardware threads available.
