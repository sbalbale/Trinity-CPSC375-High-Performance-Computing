---
aliases: [Manager-Worker, Master-Slave]
tags: [#exam/algorithm, #hpc]
sources: [lec16.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Master-Worker Pattern

> [!abstract] TL;DR Summary
> A centralized [[Dynamic Mapping]] strategy where one **Master** process manages a task pool and multiple **Worker** processes execute tasks upon request. **Load balancing** is handled automatically as faster workers process more tasks.

## Core Mechanics
1. **Initialization:** Master sets up the task pool.
2. **Distribution:** Master sends tasks to Workers as they become available/idle.
3. **Execution:** Workers perform the computation and return results.
4. **Termination:** Master signals workers to stop once the pool is empty and all results are collected.

> [!warning] Common Pitfalls
> * **Scalability:** The Master can become a **bottleneck** as the number of workers increases.
> * **Starvation:** If the Master is slow to respond, Workers remain idle (communication latency).

## Connections
* **Type of:** [[Dynamic Mapping]]
* **Used In:** [[Parallel Search]], [[Mandelbrot Set Generation]]
