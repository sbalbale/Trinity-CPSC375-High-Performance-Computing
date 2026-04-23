---
aliases: [Task Mapping, Process Mapping, Mapping (Parallel Programming), block-striped-decomposition, interleaved-decomposition]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf, lec16.pdf]
created: 2026-04-20
updated: 2025-05-15
---

# Mapping (Parallel Programming)

> [!abstract] TL;DR Summary
> The final stage of [[Foster's Methodology]] involving the assignment of agglomerated tasks to **physical processors**. The goal is to minimize execution time by balancing **load** and minimizing **communication**.

## Core Mechanics
- **Complexity:** General mapping is **NP-complete**; heuristic solutions are required.
- **Goal:** Maximize utilization (keep CPUs busy) while minimizing overhead.
- **Strategies:**
    - [[Static Mapping]]: Fixed assignment (Block, Cyclic).
    - [[Dynamic Mapping]]: Runtime assignment ([[Master-Worker Pattern]], Work Stealing).

> [!warning] Common Pitfalls
> - **Load Imbalance:** If some processors finish early while others are still working, the total execution time is determined by the slowest processor.
> - **Communication Locality:** Mapping communicating tasks to distant processors in a network can increase latency.

## Connections
* **Prerequisites:** [[Agglomeration]]
* **Goal:** [[Load Balancing]]
