---
aliases: [omp critical, omp barrier, omp atomic]
tags: [openmp, synchronization, performance]
sources: [Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# OpenMP Synchronization

> [!abstract]
> **OpenMP Synchronization** directives control the execution order of threads and ensure safe access to shared resources. Proper synchronization is required to prevent [[race-condition]]s but must be used carefully to avoid performance bottlenecks.

## Core Mechanics

### Critical Sections
> [!code] Syntax
> ```c
> #pragma omp critical
> {
>     // Shared resource access
> }
> ```
Ensures that the enclosed block is executed by only one thread at a time. It effectively serializes that part of the code.

### Barriers
> [!code] Syntax
> ```c
> #pragma omp barrier
> ```
Synchronizes all threads in the team. No thread can proceed beyond the barrier until all threads have reached it.

### Atomics
> [!code] Syntax
> ```c
> #pragma omp atomic
> x++;
> ```
A lighter-weight version of `critical` that applies only to a single memory update. It leverages hardware-level atomic instructions.

> [!warning] Serialization Overhead
> Frequent use of `critical` sections can destroy parallel performance by forcing threads to wait in a queue. **Reductions** or **Atomics** should be used whenever possible instead.

## Connections
* **Prerequisites:** [[race-condition]], [[openmp]].
* **Used In:** [[reduction-operation]].
* **Contrasts With:** Distributed memory synchronization (e.g., `MPI_Barrier`).
