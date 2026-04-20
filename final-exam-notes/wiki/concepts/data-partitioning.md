---
aliases: [Block Distribution, Cyclic Distribution, Block-Cyclic Distribution]
tags: 
  - #exam/theory
sources: [lec21.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Data Partitioning

> [!abstract] TL;DR Summary
> **Data Partitioning** determines how a large data structure (like an array or matrix) is divided and assigned to multiple processes in a distributed system to balance workload and minimize communication.

## Core Mechanics

> [!example] Partitioning Types
> For a 12-element array among 3 processes:
> - **Block Partitioning:** Contiguous chunks (P0: 0-3, P1: 4-7, P2: 8-11).
> - **Cyclic Partitioning:** Round-robin elements (P0: 0,3,6,9; P1: 1,4,7,10; P2: 2,5,8,11).
> - **Block-Cyclic Partitioning:** Round-robin blocks of elements.

## Connections
* **Used In:** [[Collective Communication]] (Scatter/Gather operations).
