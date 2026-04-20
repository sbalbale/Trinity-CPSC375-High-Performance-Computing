---
aliases: [Open Multi-Processing]
tags: 
  - #exam/tool
  - #CPSC375
sources: [Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# OpenMP

> [!abstract] TL;DR Summary
> **OpenMP** (Open Multi-Processing) is the industry-standard API for **shared-memory** parallel programming. It uses compiler directives, runtime routines, and environment variables to implement the [[fork-join-model]].

## Core Mechanics
* **Directives:** `#pragma omp parallel`, `#pragma omp for`, `#pragma omp critical`.
* **Runtime Functions:** 
    - `omp_get_thread_num()`: ID of the current thread.
    - `omp_set_num_threads(n)`: Sets the number of threads for the next region.
    - `omp_get_num_threads()`: Returns the number of threads in the current region.
* **Compilation:** Requires the `-fopenmp` flag (e.g., `gcc -fopenmp main.c`).

> [!code] Simple OpenMP Program
> ```c
> #include <omp.h>
> #include <stdio.h>
> int main() {
>     #pragma omp parallel
>     {
>         printf("Hello from thread %d\n", omp_get_thread_num());
>     }
> }
> ```

## Connections
* **Architecture:** [[shared-memory-model]].
* **Logic:** [[fork-join-model]].
* **Components:** [[parallel-region]], [[reduction-operation]], [[data-sharing-clauses]].
* **Contrasts With:** [[mpi]] (distributed memory).
