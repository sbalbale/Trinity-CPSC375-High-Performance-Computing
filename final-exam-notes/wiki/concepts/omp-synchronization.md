---
aliases: [omp critical, omp barrier, omp atomic, omp single, omp master]
tags: [openmp, synchronization, performance]
sources: [Getting Started with OpenMP Programming.txt, More on OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# OpenMP Synchronization

> [!abstract]
> **OpenMP Synchronization** directives control the execution order of threads and ensure safe access to shared resources. They include primitives for mutual exclusion, team synchronization, and single-thread execution.

## Core Mechanics

### Mutual Exclusion
- **Critical Section**: `#pragma omp critical`. Only one thread executes the block at a time.
- **Atomic**: `#pragma omp atomic`. Fast, hardware-level protection for a single memory update (e.g., `x++`).

### Team Synchronization
- **Barrier**: `#pragma omp barrier`. All threads wait until the entire team reaches this point.

### Single-Thread Execution
- **Master**: `#pragma omp master`. Only the master thread (ID 0) executes. No implicit barrier.
- **Single**: `#pragma omp single`. Any one thread executes. An **implicit barrier** follows unless `nowait` is used.

> [!warning] Serialization Overhead
> Frequent use of `critical` sections can destroy parallel performance. **Reductions** or **Atomics** should be used whenever possible instead.

## Connections
* **Prerequisites:** [[race-condition]], [[openmp]].
* **Used In:** [[reduction-operation]], [[omp-tasks-sections]].
* **Contrasts With:** Distributed memory synchronization (e.g., `MPI_Barrier`).
