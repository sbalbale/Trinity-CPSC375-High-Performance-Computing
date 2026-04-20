---
aliases: [Strassen's Multiplication, $O(n^{2.81})$ Matrix Mult]
tags: [algorithm, matrix, divide-and-conquer]
sources: [lec30.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Strassen's Algorithm

> [!abstract]
> **Strassen's Algorithm** is a divide-and-conquer matrix multiplication method that reduces the algebraic complexity from $O(n^3)$ to **$O(n^{2.81})$**. It does this by reducing the number of scalar multiplications required for a $2 \times 2$ matrix multiplication from 8 to **7**.

## Core Mechanics

### Seven Products
Instead of standard dot products, Strassen defines 7 intermediate products ($P_1$ to $P_7$) involving sums and differences of submatrices:
- $P_1 = (A_{11} + A_{22})(B_{11} + B_{22})$
- $P_2 = (A_{21} + A_{22})B_{11}$
- ...
- $P_7 = (A_{12} - A_{22})(B_{21} + B_{22})$

The final matrix $C$ is then constructed using these 7 products through several additions and subtractions.

> [!equation] Time Complexity
> $$T(n) = 7 T(n/2) + O(n^2) \implies O(n^{\log_2 7}) \approx O(n^{2.81})$$

> [!warning] Trade-offs
> - **Additions**: Strassen's requires significantly more additions (18 vs 4).
> - **Numerical Stability**: It is less numerically stable than the standard algorithm for floating-point values.
> - **Crossover Point**: Due to the overhead of additions, it is only faster than standard multiplication for very large matrices.

## Connections
* **Prerequisites:** [[shared-memory-model]] (Divide-and-Conquer).
* **Used In:** Large-scale linear algebra where multiplication is the dominant cost.
* **Contrasts With:** Standard $O(n^3)$ multiplication.
* **Author:** [[strassen]].
