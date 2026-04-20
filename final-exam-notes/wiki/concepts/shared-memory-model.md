---
aliases: [Shared Memory Architecture, SMA]
tags: 
  - #exam/theory
  - #course_hpc
sources: [lec20.pdf, Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Shared Memory Model

> [!abstract] TL;DR Summary
> A parallel programming model where multiple processors or cores **share a single global address space**. Threads communicate implicitly by **reading and writing shared variables** in this common memory.

## Core Mechanics

> [!equation] Communication Pattern
> - **Implicit Communication:** Data is exchanged by updating shared memory locations.
> - **Data Visibility:** Changes made by one thread are (potentially) visible to all others.
> - **Synchronization:** Requires explicit mechanisms (e.g., mutexes, critical sections) to prevent race conditions.

> [!warning] Common Pitfalls
> - **Race Conditions:** Occur when multiple threads access the same memory location simultaneously and at least one access is a write.
> - **Cache Coherency:** Hardware must ensure that changes in one processor's cache are reflected in others.
> - **Scalability:** Bus contention and memory bandwidth can limit performance as the number of cores increases.

## Implementations & Examples

> [!example] Comparison with Distributed Memory
> | Feature | Shared Memory | Distributed Memory (MPI) |
> | --- | --- | --- |
> | **Address Space** | Single, Global | Multiple, Local |
> | **Communication** | Implicit (Variables) | Explicit (Messages) |
> | **Hardware** | Multi-core CPU, SMP | Cluster, Networked PCs |

> [!code] OpenMP Shared Variable
> ```c
> int x = 10;
> #pragma omp parallel shared(x)
> {
>     // All threads see the same x
>     printf("Thread %d sees x = %d\n", omp_get_thread_num(), x);
> }
> ```

## Connections
* **Prerequisites:** [[Parallel Region]]
* **Used In:** [[openmp]], [[Race Condition]]
* **Contrasts With:** [[Distributed Memory Model]]
