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
