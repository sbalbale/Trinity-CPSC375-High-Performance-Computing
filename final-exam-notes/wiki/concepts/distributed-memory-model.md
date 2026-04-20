---
aliases: [Private Memory, Message Passing Architecture, Distributed Memory Model]
tags: [architecture, programming, distributed]
sources: [lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Distributed Memory Model

> [!abstract]
> The **Distributed Memory Model** is a parallel architecture where each processor has its own **private, local memory**. Processors communicate and share data by explicitly sending and receiving messages over an interconnect.

## Core Mechanics

- **Private Scope**: Variables in one process are not visible to any other process.
- **Message Passing**: Data movement is handled by software libraries like **MPI**.
- **Scalability**: This model is highly scalable (e.g., to thousands of nodes) because it avoids the memory bus contention inherent in shared-memory systems.
- **Data Locality**: The programmer must explicitly manage where data resides and how it is moved.

> [!warning] Complexity
> Distributed memory programming is generally more complex than shared-memory because the programmer must handle all communication, data partitioning, and synchronization manually.

## Implementations & Examples

> [!example] Clusters
> A typical Linux cluster (like the [[pine-cluster]]) is a distributed-memory system. Each node has its own RAM and CPU; nodes communicate via Ethernet or InfiniBand.

## Connections
* **Prerequisites:** Basic computer architecture.
* **Used In:** [[mpi]].
* **Contrasts With:** [[shared-memory-model]].
