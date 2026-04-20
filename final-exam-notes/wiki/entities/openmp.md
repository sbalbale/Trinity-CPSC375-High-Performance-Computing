---
aliases: [Open Multi-Processing]
tags: 
  - #exam/tool
  - #CPSC375
sources: [Getting Started with OpenMP Programming.txt, More on OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# OpenMP

> [!abstract] TL;DR Summary
> **OpenMP** (Open Multi-Processing) is the industry-standard API for **shared-memory** parallel programming. It uses compiler directives, runtime routines, and environment variables to implement the [[fork-join-model]].

## Core Mechanics
* **Directives:** 
    - `#pragma omp parallel`: Basic team creation.
    - `#pragma omp for`: Loop worksharing.
    - `#pragma omp critical` / `atomic`: Synchronization.
    - `#pragma omp task`: Dynamic tasking.
* **Runtime Functions:** 
    - `omp_get_thread_num()` / `omp_set_num_threads(n)`.
    - `omp_get_wtime()`: High-resolution wall-clock timer.
* **Compilation:** Requires the `-fopenmp` flag.

## Connections
* **Architecture:** [[shared-memory-model]].
* **Components:** [[parallel-region]], [[omp-synchronization]], [[omp-scheduling]], [[omp-tasks-sections]].
* **Contrasts With:** [[mpi]] (distributed memory).
