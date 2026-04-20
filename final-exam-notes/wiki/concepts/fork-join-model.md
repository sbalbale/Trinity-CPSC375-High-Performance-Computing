---
aliases: [Master-Team Model, Thread Forking]
tags: 
  - #exam/theory
  - #course_hpc
sources: [lec20.pdf, Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Fork-Join Model

> [!abstract] TL;DR Summary
> A standard execution pattern for shared-memory parallel programs where a **single master thread** spawns a team of worker threads for parallel regions and **synchronizes back** to sequential execution.

## Core Mechanics

> [!equation] Execution Stages
> 1. **Master Thread:** Starts execution sequentially.
> 2. **Fork:** Upon reaching a parallel region, the master creates a team of worker threads.
> 3. **Parallel Execution:** All threads in the team execute code within the parallel region.
> 4. **Implicit Barrier:** Threads wait for each other at the end of the region.
> 5. **Join:** Threads are terminated or parked, and only the master thread continues.

> [!warning] Common Pitfalls
> - **Thread Creation Overhead:** Frequent fork/join operations can degrade performance.
> - **Implicit Synchronization:** Forgetting the automatic join/barrier at the end of a region can lead to misunderstandings of program flow.
> - **Imbalanced Workload:** If one thread takes much longer than others, all threads must wait at the join point, reducing efficiency.

## Implementations & Examples

> [!example] OpenMP Fork-Join
> In OpenMP, the `#pragma omp parallel` directive triggers the fork-join process.
> - **Master Thread ID:** 0
> - **Worker Thread IDs:** 1 to $N-1$

> [!code] OpenMP Syntax
> ```c
> // Master thread starts
> #pragma omp parallel
> {
>     // TEAM OF THREADS FORKED HERE
>     // Each thread executes this block
> }
> // TEAM JOINS HERE, only master continues
> ```

## Connections
* **Prerequisites:** [[Parallel Region]]
* **Used In:** [[openmp]], [[Dynamic Mapping]]
* **Contrasts With:** [[Distributed Memory Model]]
