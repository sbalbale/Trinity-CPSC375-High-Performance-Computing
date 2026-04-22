---
aliases: [Block Distribution, Cyclic Distribution, Block-Cyclic Distribution, Data Partitioning]
tags: 
  - #exam/theory
sources: [lec21.pdf, HW21_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Data Partitioning

> [!abstract] TL;DR Summary
> **Data Partitioning** determines how a large data structure (like an array or matrix) is divided and assigned to multiple processes in a distributed system to balance workload and minimize communication.

## Core Mechanics

> [!example] Partitioning Types
> For a 16-element array among 4 processes (P0-P3):
> - **Block Partitioning:** Contiguous chunks (P0: 0-3, P1: 4-7, P2: 8-11, P3: 12-15).
> - **Cyclic Partitioning:** Round-robin elements (P0: 0,4,8,12; P1: 1,5,9,13; P2: 2,6,10,14; P3: 3,7,11,15).
> - **Block-Cyclic Partitioning:** Round-robin blocks of elements (e.g., block size 2).

### Comparison of Strategies

| Strategy | Best For... | Advantages | Disadvantages |
|---|---|---|---|
| **Block** | **Uniform work** per element | Excellent **locality**; minimal communication (only boundary exchange) | Poor **load balancing** if work varies (e.g., triangular loops) |
| **Cyclic** | **Variable work** per element | Better **load balancing**; statistically distributes heavy work | Poor **locality**; may increase communication frequency |
| **Block-Cyclic** | **Irregular work** | Tunable middle ground between locality and load balance | More complex to implement |

> [!info] Trade-off
> Use **Block** when cache performance and minimal messaging are priorities and work is even. Use **Cyclic** when some elements take much longer than others (e.g., sparse matrices) to ensure no process is left idle.

## Connections
* **Used In:** [[Collective Communication]] (Scatter/Gather operations).
