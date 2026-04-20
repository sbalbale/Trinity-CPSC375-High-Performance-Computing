---
aliases: [OpenMP Lab Summary, Introduction to OpenMP Summary]
tags: [#lecture/detailed, #CPSC375, #openmp]
sources: [Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lab: Getting Started with OpenMP

> [!abstract]
> This lab provides a hands-on introduction to **OpenMP**, focusing on the creation of parallel regions, loop distribution, and the prevention of race conditions through synchronization and data-sharing clauses.

## Basic Syntax and Compilation
- **Header**: `#include <omp.h>`
- **Pragma**: `#pragma omp parallel`
- **Compiler Flag**: `gcc -fopenmp`
- **Thread Control**: `omp_set_num_threads(4)` and `omp_get_thread_num()`.

## Parallel Loops
The directive `#pragma omp parallel for` automatically divides iterations of a `for` loop among threads. Iterations are not guaranteed to execute in order.

## Race Conditions and Synchronization
When multiple threads attempt to update a shared variable simultaneously (e.g., `sum += 1`), the result is unpredictable.

### Critical Sections
> [!code] Critical Syntax
> ```c
> #pragma omp critical
> {
>     sum += 1;
> }
> ```
Ensures only one thread enters the block at a time. Safe but slow due to serialization overhead.

### Reductions
> [!code] Reduction Syntax
> ```c
> #pragma omp parallel for reduction(+:sum)
> ```
Threads keep private copies of `sum` and combine them using the `+` operator at the end of the region. Highly efficient.

## Data-Sharing Clauses
| Clause | Behavior |
| :--- | :--- |
| `shared(x)` | One instance of `x` is shared by all threads (default for global). |
| `private(x)` | Each thread gets its own uninitialized copy of `x`. |
| `firstprivate(x)` | Each thread gets its own copy of `x`, initialized with the value from before the region. |
| `lastprivate(x)` | The value of `x` from the logically last iteration of a loop is copied back to the original variable. |
