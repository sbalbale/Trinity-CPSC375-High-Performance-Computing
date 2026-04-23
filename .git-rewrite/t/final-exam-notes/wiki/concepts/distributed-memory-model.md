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

### Architecture Components
In a distributed-memory system, each node typically contains:
- **Processor**: A CPU or core that executes instructions.
- **Local Memory**: RAM that is private to that specific processor.
- **Interconnect**: A network (e.g., InfiniBand, Ethernet) that allows nodes to exchange messages.

### Key Characteristics
1. **No Common Address Space**: There is no global memory; pointers from one process are meaningless to another.
2. **Explicit Data Transfer**: Every piece of shared data must be bundled into a message, sent, and received.
3. **Unique Identification**: Each process has a unique ID (rank) used to address it in the network.
4. **Scalability**: Scaling is achieved by adding more independent nodes, avoiding the physical limits of a single shared memory bus.

## Implementations & Examples

> [!example] Clusters
> A typical Linux cluster (like the [[pine-cluster]]) is a distributed-memory system. Each node has its own RAM and CPU; nodes communicate via Ethernet or InfiniBand.

## Connections
* **Prerequisites:** Basic computer architecture.
* **Used In:** [[mpi]].
* **Contrasts With:** [[shared-memory-model]].
