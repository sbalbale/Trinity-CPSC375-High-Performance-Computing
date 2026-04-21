---
aliases: [Shared Memory Architecture, SMA]
tags: 
  - #exam/theory
  - #course_hpc
sources: [lec20.txt, Getting Started with OpenMP Programming.txt, lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Shared Memory Model

> [!abstract]
> A parallel programming model where multiple processors or cores **share a single global address space**. Threads communicate implicitly by **reading and writing shared variables** in this common memory.

## Core Mechanics

- **Implicit Communication:** Data is exchanged by updating memory locations rather than sending explicit messages.
- **Synchronization**: Requires explicit primitives (e.g., [[omp-synchronization]]) to prevent [[race-condition]]s.
- **Scalability**: Fundamentally limited by memory bus bandwidth and contention as core counts increase.

> [!warning] Cache Coherency
> In shared-memory hardware, each processor has its own cache. Maintaining a consistent view of the global memory requires complex **cache coherence protocols** (like Write-Through or Write-Once).

## Key Characteristics
1. **Single Memory Address Space**: All processors access the same physical memory; no explicit data transfer (sends/receives) is needed.
2. **Implicit Data Sharing**: Communication happens through shared variables, which is natural for iterative methods and shared-data algorithms.
3. **Synchronization**: Processors coordinate through shared variables, requiring careful management to avoid conflicts.

## Implementations & Examples

### Shared-Memory (OpenMP) vs. Message-Passing (MPI)
| Feature | Shared Memory (OpenMP) | Message-Passing (MPI) |
| :--- | :--- | :--- |
| **Address Space** | Single, Global | Multiple, Local |
| **Communication** | Implicit (Variables) | Explicit (Messages) |
| **Programming** | Incremental / Gradual | Giant Step / Redesign |
| **Thread Lifecycle** | Dynamic (Fork/Join) | Static (Always Active) |
| **Hardware** | Multi-core CPU, SMP | Cluster, Networked PCs |
| **Debugging** | Difficult (Race Conditions) | Easier (Explicit Flow) |

> [!example] Abstractions
> - **Pure Shared Memory**: Physically shared RAM (e.g., a laptop CPU).
> - **Distributed Shared Memory (DSM)**: Physically distributed but logically shared via an abstraction layer.

## Connections
* **Prerequisites:** [[fork-join-model]].
* **Used In:** [[openmp]], [[distributed-shared-memory]].
* **Contrasts With:** [[distributed-memory-model]].
