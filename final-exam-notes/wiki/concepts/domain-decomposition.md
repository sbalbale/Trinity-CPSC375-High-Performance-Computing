---
aliases: [Data Decomposition, Domain Decomposition]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Domain Decomposition

> [!abstract] TL;DR Summary
> A [[Partitioning]] strategy where the **data** (e.g., an array, matrix, or spatial grid) is divided into pieces, and each task performs the same operation on its piece of data.

## Core Mechanics
- **Focus:** Data structures.
- **Example:** In a weather model, the 3D atmosphere grid is split into sub-grids processed by different processors.

> [!warning] Common Pitfalls
> - **Boundary Conditions:** Communication is often required to exchange data at the boundaries of decomposed domains.

## Connections
* **Type Of:** [[Partitioning]]
* **Contrasts With:** [[Functional Decomposition]]
