---
aliases: [omp critical, omp barrier, omp atomic, omp single, omp master, omp-runtime-functions]
tags: [openmp, synchronization, performance]
sources: [Getting Started with OpenMP Programming.txt, More on OpenMP Programming.txt, HW19_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# OpenMP Synchronization

> [!abstract]
> **OpenMP Synchronization** directives control the execution order of threads and ensure safe access to shared resources. They include primitives for mutual exclusion, team synchronization, and single-thread execution.

## Core Mechanics

### Mutual Exclusion
- **Critical Section**: `#pragma omp critical`. Only one thread executes the block at a time.
- **Atomic**: `#pragma omp atomic`. Fast, hardware-level protection for a single memory update (e.g., `x++`).

#### Atomic vs. Critical Comparison

| Feature | `#pragma omp critical` | `#pragma omp atomic` |
|---|---|---|
| **Mechanism** | Software **mutex lock** (OS-level) | Single **hardware instruction** |
| **Overhead** | High (kernel calls, context switching) | **Minimal** (CPU-level) |
| **Scope** | Serializes an **entire code block** | Serializes a **single memory operation** |
| **Performance** | Much slower for fine-grained updates | **Significantly faster** for simple scalar updates |

### Team Synchronization
- **Barrier**: `#pragma omp barrier`. All threads wait until the entire team reaches this point.

### Single-Thread Execution
| Directive | Executing Thread | Implicit Barrier at End? |
| :--- | :--- | :--- |
| **`#pragma omp master`** | Only Thread 0 | **No** (threads skip and continue) |
| **`#pragma omp single`** | Any one thread | **Yes** (all threads wait) |

> [!warning] Serialization Overhead
> Frequent use of `critical` sections can destroy parallel performance. **Reductions** or **Atomics** should be used whenever possible instead.

### Best Practices
1. **Use the simplest mechanism**: `atomic` is faster than `critical`.
2. **Minimize Overhead**: Place synchronization outside inner loops and batch operations.
3. **Control Barriers**: Use `nowait` with `single` or `for` to skip the implicit barrier if data dependencies allow it.

## Connections
* **Prerequisites:** [[race-condition]], [[openmp]].
* **Used In:** [[reduction-operation]], [[omp-tasks-sections]].
* **Contrasts With:** Distributed memory synchronization (e.g., `MPI_Barrier`).
