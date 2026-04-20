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

## Implementations & Examples

### Comparison with Distributed Memory
| Feature | Shared Memory | Distributed Memory (MPI) |
| :--- | :--- | :--- |
| **Address Space** | Single, Global | Multiple, Local |
| **Communication** | Implicit (Variables) | Explicit (Messages) |
| **Hardware** | Multi-core CPU, SMP | Cluster, Networked PCs |

> [!example] Abstractions
> - **Pure Shared Memory**: Physically shared RAM (e.g., a laptop CPU).
> - **Distributed Shared Memory (DSM)**: Physically distributed but logically shared via an abstraction layer.

## Connections
* **Prerequisites:** [[fork-join-model]].
* **Used In:** [[openmp]], [[distributed-shared-memory]].
* **Contrasts With:** [[distributed-memory-model]].
