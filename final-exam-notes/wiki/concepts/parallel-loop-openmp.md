---
aliases: [omp for, Loop Parallelization]
tags: [openmp, programming, optimization]
sources: [Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Loops in OpenMP

> [!abstract]
> **Parallel Loops** in OpenMP use the `#pragma omp for` or `#pragma omp parallel for` directive to automatically distribute the iterations of a `for` loop among a team of threads. This is the most common way to achieve data parallelism in shared-memory systems.

## Core Mechanics

- **Worksharing**: The compiler analyzes the loop bounds and assigns chunks of iterations to different threads.
- **Constraints**: 
    - The loop must have a **canonical form** (e.g., `for (i = 0; i < n; i++)`).
    - The number of iterations must be determinable at the start of the loop.
- **Implicit Barrier**: There is an implicit barrier at the end of a parallel loop (unless `nowait` is specified), ensuring all threads finish their iterations before moving on.

> [!warning] Iteration Dependency
> Parallel loops must not have **data dependencies** between iterations. If iteration $i$ depends on a value calculated in iteration $i-1$, the loop cannot be parallelized without refactoring or synchronization.

## Implementations & Examples

> [!code] Combined Directive
> ```c
> #pragma omp parallel for
> for (int i = 0; i < 1000; i++) {
>     A[i] = B[i] + C[i];
> }
> ```

## Connections
* **Prerequisites:** [[parallel-region]], [[openmp]].
* **Used In:** [[reduction-operation]], [[matrix-multiplication-optimization]].
* **Contrasts With:** Manual thread ID-based work division.
