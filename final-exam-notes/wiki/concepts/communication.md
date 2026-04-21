---
aliases: [Inter-task Communication, Communication]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Communication

> [!abstract] TL;DR Summary
> The second stage of [[Foster's Methodology]] defining how tasks exchange data. Effective design minimizes **communication overhead** to prevent it from becoming a **bottleneck**.

## Communication Checklist
An effective design must satisfy these criteria:
1. **Balanced Communication**: Data exchange should be evenly distributed among tasks; no single task should be a communication bottleneck.
2. **Sparse Communication**: Each task should communicate with only a few neighbors (local communication).
3. **Concurrent Communication**: Multiple tasks should be able to exchange data simultaneously to utilize full network bandwidth.
4. **Overlap**: Design should allow for **concurrent computation**, where communication is hidden behind useful work.

## Core Mechanics
- **Local Communication:** Task interacts with a small neighborhood of tasks.
- **Global Communication:** Task interacts with many or all other tasks (e.g., reductions, broadcasts).
- **Synchronous vs. Asynchronous:** Blocking vs. non-blocking data exchange.

> [!warning] Common Pitfalls
> - **High Overhead:** Small tasks with high communication requirements lead to poor efficiency.
> - **Serialization:** Global communication can serialize execution if not managed (e.g., all tasks waiting for a single root).

## Connections
* **Prerequisites:** [[Partitioning]]
* **Used In:** [[Agglomeration]]
* **Contrasts With:** Computation time (see Speedup)
