---
aliases: [Latency and Bandwidth, Messaging Costs]
tags: [performance, theory, mpi]
sources: [lec23.txt, Homework 24.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Communication Overhead

> [!abstract]
> **Communication Overhead** is the time spent moving data between processors, which does not contribute directly to the computation. It is a major factor in the [[overhead-parallel-computing]] of distributed systems.

## Core Mechanics

- **Components**:
    - **Latency ($\lambda$)**: The fixed time to initiate a message.
    - **Bandwidth ($\beta$)**: The rate at which data is transferred once initiated.
- **Impact**: High overhead reduces the [[computation-to-communication-ratio]], making parallelization less efficient.

## Connections
* **Prerequisites:** [[distributed-memory-model]].
* **Used In:** [[parallel-speedup]] analysis, [[isoefficiency-function]].
* **Contrasts With:** [[synchronization-overhead]].
