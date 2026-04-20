---
aliases: [Task Mapping, Process Mapping]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Mapping (Parallel Programming)

> [!abstract] TL;DR Summary
> The final stage of [[Foster's Methodology]] involving the assignment of agglomerated tasks to **physical processors**. The goal is to maximize processor utilization through **Load Balancing**.

## Core Mechanics
- **Static Mapping:** Tasks assigned before runtime (e.g., Block, Cyclic, Block-Cyclic distribution).
- **Dynamic Mapping:** Tasks assigned during runtime (e.g., Work Stealing, Task Queues).

> [!warning] Common Pitfalls
> - **Load Imbalance:** If some processors finish early while others are still working, the total execution time is determined by the slowest processor.
> - **Communication Locality:** Mapping communicating tasks to distant processors in a network can increase latency.

## Connections
* **Prerequisites:** [[Agglomeration]]
* **Goal:** [[Load Balancing]]
