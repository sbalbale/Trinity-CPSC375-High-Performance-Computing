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

### Detailed Phases
1. **Initialization**: Only the **master thread** is active, executing sequential code (I/O, setup).
2. **Fork Phase**: The master thread creates (or awakens) a team of worker threads.
3. **Parallel Phase**: Multiple threads work concurrently, each with its own program counter and stack.
4. **Join Phase**: An implicit barrier at the end of the region forces threads to synchronize. Worker threads are suspended or destroyed.
5. **Finalization**: Only the master thread gathers results and continues sequential execution.

> [!info] Dynamic Property
> Unlike MPI, where the process count is typically fixed, the **thread count in OpenMP is dynamic** and changes during execution as regions are entered and exited. The program always begins and ends in a single-threaded state.

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
