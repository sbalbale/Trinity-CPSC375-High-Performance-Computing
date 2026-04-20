---
aliases: [Parallel Overhead]
tags: [#exam/theory, #hpc]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Overhead (Parallel Computing)

> [!abstract] TL;DR Summary
> The total time spent by all processors in a parallel system that is **not** spent performing useful computation. It is the reason why speedup is usually sublinear.

## Core Mechanics
* **Formula:** $T_o = p \cdot T_p - T_s$.

### Primary Sources
1. **Inter-process Communication:** Time spent sending and receiving messages.
2. **Synchronization:** Time spent waiting at barriers or for locks.
3. **Idleness:** Time processors spend waiting due to **Load Imbalance**.
4. **Extra Computation:** Logic required for the parallel version (e.g., partitioning, collecting results).

> [!warning] Common Pitfalls
> * **Ignoring Hidden Overheads:** Cache misses caused by data sharing or OS context switching can be significant but hard to measure directly.

## Connections
* **Causes:** [[Sublinear Speedup]]
* **Managed By:** [[Load Balancing]], [[Agglomeration]]
