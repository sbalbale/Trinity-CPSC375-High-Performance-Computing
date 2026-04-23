---
aliases: [omp schedule, Static vs Dynamic Scheduling, Task Scheduling]
tags: [openmp, performance, load-balancing]
sources: [More on OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# OpenMP Loop Scheduling

> [!abstract]
> **OpenMP Loop Scheduling** defines how iterations of a parallel loop are distributed among threads. Choosing the right scheduling policy is critical for **load balancing** in programs with irregular workloads.

## Core Mechanics

### Scheduling Policies
`#pragma omp parallel for schedule(type [, chunk])`

- **Static**: Iterations are divided into pieces of size `chunk` and assigned to threads in round-robin fashion.
    - **Pros**: Minimal runtime overhead.
    - **Cons**: Can lead to load imbalance if iterations have different work amounts.
- **Dynamic**: Iterations are assigned to threads as they become free.
    - **Pros**: Excellent for **load balancing** irregular loops.
    - **Cons**: Higher runtime overhead due to task management.
- **Guided**: Similar to dynamic, but chunk sizes decrease over time.
- **Auto**: Runtime chooses the best policy.

> [!warning] Load Imbalance
> If one thread handles a set of "heavy" iterations while others finish quickly, the total parallel time is determined by the slow thread. **Dynamic scheduling** is the primary solution for this.

## Implementations & Examples

> [!example] Dynamic Scheduling
> ```c
> #pragma omp parallel for schedule(dynamic, 4)
> for (int i = 0; i < n; i++) {
>     expensive_function(i); 
> }
> ```
Threads take 4 iterations at a time. Once a thread finishes its 4, it requests the next available set.

## Connections
* **Prerequisites:** [[parallel-loop-openmp]], [[load-balancing]].
* **Used In:** Irregular algorithms like [[sieve-of-eratosthenes]] (if implemented with OpenMP).
* **Contrasts With:** [[static-mapping]] in MPI.
