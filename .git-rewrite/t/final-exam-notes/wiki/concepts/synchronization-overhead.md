---
aliases: [Waiting Time, Barrier Costs]
tags: [performance, theory, synchronization]
sources: [Homework 25.pdf, lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Synchronization Overhead

> [!abstract]
> **Synchronization Overhead** is the time processors spend waiting for each other at synchronization points, such as [[mpi-barrier]] or `#pragma omp critical`. It is a component of [[overhead-parallel-computing]] that arises from load imbalance or serial sections.

## Core Mechanics

- **Idle Time**: Processors that finish their work early must wait for the slowest processor to reach the barrier.
- **Lock Contention**: In shared memory, multiple threads competing for the same lock (e.g., `critical`) results in threads being blocked.

## Connections
* **Prerequisites:** [[load-balancing]].
* **Used In:** Analysis of [[odd-even-transposition-sort]].
* **Contrasts With:** [[communication-overhead]].
