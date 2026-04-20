---
aliases: [Work Distribution]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Load Balancing

> [!abstract] TL;DR Summary
> The process of distributing tasks across processors such that all processors remain busy for the duration of the computation. Minimizing **idle time** is essential for high efficiency.

## Core Mechanics
- **Static:** Fixed distribution (e.g., data split equally).
- **Dynamic:** On-the-fly distribution (e.g., master-worker pattern).

> [!warning] Common Pitfalls
> - **Scheduling Overhead:** Dynamic load balancing introduces extra communication and computation to manage task assignments.
> - **Data Locality:** Moving tasks to balance load might require moving their associated data, increasing communication costs.

## Connections
* **Goal Of:** [[Mapping (Parallel Programming)]]
* **Related To:** Speedup, Efficiency
