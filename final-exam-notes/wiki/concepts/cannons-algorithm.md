---
aliases: [Skew-and-Shift Matrix Mult]
tags: [algorithm, matrix, distributed, mpi]
sources: [lec31.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Cannon's Algorithm

> [!abstract]
> **Cannon's Algorithm** is a distributed matrix multiplication algorithm for 2D meshes. It uses an **initial skewing** alignment phase followed by a series of **shift-multiply** steps to compute the product in $O(n^3/p)$ time with $O(n/\sqrt{p})$ communication ratio.

## Core Mechanics

### Phase 1: Alignment (Skewing)
Before multiplication starts, blocks must be aligned so that each processor has a valid pair of submatrices to multiply:
- **Matrix A**: Circularly shift each row $i$ left by $i$ positions.
- **Matrix B**: Circularly shift each column $j$ up by $j$ positions.

### Phase 2: Shift-Multiply
Perform $\sqrt{p}$ iterations:
1. **Multiply**: $C_{ij} = C_{ij} + (A_{local} \times B_{local})$.
2. **Shift**: 
    - Circularly shift each row of A left by 1.
    - Circularly shift each column of B up by 1.

> [!warning] Implementation Complexity
> Cannon's is often difficult to implement due to the complex circular shifting logic required for the initial alignment, especially if the number of processors is not a perfect square.

## Connections
* **Prerequisites:** [[checkerboard-decomposition]].
* **Used In:** Distributed-memory matrix multiplication.
* **Contrasts With:** [[foxs-algorithm]] (broadcast instead of skew).
