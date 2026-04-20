---
aliases: [2D Grid Decomposition, 2D Partitioning]
tags: [mapping, data-partitioning, matrix]
sources: [lec31.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Checkerboard Decomposition

> [!abstract]
> **Checkerboard Decomposition** (or 2D partitioning) divides a matrix into a $\sqrt{p} \times \sqrt{p}$ grid of square blocks, assigning each block to one of $p$ processors. It is more scalable than 1D partitioning for matrix operations.

## Core Mechanics

- **Mapping**: Processor $P_{i,j}$ (in a 2D logical grid) is responsible for the submatrix block $C_{i,j}$ of the product $C = A \times B$.
- **Data Access**: To compute $C_{i,j}$, the processor needs all blocks in row $i$ of matrix A and all blocks in column $j$ of matrix B.
- **Scalability**: Reduces communication volume. In 1D partitioning, each process communicates $O(n^2)$ elements. In 2D partitioning, each process communicates $O(n^2/\sqrt{p})$ elements.

> [!equation] Computation-to-Communication Ratio
> - **1D Striped**: $\text{Ratio} \approx \frac{n}{p}$
> - **2D Checkerboard**: $\text{Ratio} \approx \frac{n}{\sqrt{p}}$
As $p$ increases, the 2D decomposition maintains a better balance of work to data movement.

## Connections
* **Prerequisites:** [[data-partitioning]].
* **Used In:** [[cannons-algorithm]], [[foxs-algorithm]], [[summa-algorithm]].
* **Contrasts With:** [[block-striped-decomposition]] (1D).
