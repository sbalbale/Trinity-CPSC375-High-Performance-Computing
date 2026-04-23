---
aliases: [DSM, Global Address Space]
tags: [architecture, dsm, parallel-systems]
sources: [lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Distributed Shared-Memory

> [!abstract]
> **Distributed Shared-Memory (DSM)** systems are computer architectures where memory is physically distributed across different processors but **logically shared** in a global address space. This allows for easier programming than distributed-memory systems.

## Core Mechanics

- **Abstraction Layer**: A software or hardware layer manages the mapping of global addresses to physical memory locations.
- **Ease of Use**: Programming for DSM systems feels like programming for a single shared-memory machine, avoiding explicit `MPI_Send` and `MPI_Recv`.
- **Complexity**: The system must handle **data locality** and **cache coherence** across multiple machines or nodes.

> [!warning] Performance Challenges
> - **False Sharing**: Two processors modify different variables located in the same cache line, causing unnecessary data movement.
> - **Latency**: Accessing memory on a remote node is significantly slower than local memory (NUMA effects).

## Implementations & Examples

> [!example] System Types
> 1. **Hardware-based DSM**: Complex hardware implements cache coherence (e.g., SGI Origin).
> 2. **Software-based DSM**: Operating system or runtime library manages memory pages (e.g., TreadMarks).

## Connections
* **Prerequisites:** [[shared-memory-model]], [[distributed-memory-model]].
* **Used In:** [[cache-coherence-protocols]].
* **Contrasts With:** [[mpi]] (purely distributed-memory systems).
