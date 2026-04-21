---
aliases: [Matrix Reference, Parallel Mult Sheet]
tags: [#exam/algorithm, #prep/moc, #course_hpc]
sources: [lec30.md, lec31.md]
created: 2026-04-20
updated: 2026-04-20
---

# Cheat Sheet: Matrix Multiplication

> [!abstract]
> Summary of sequential optimizations and distributed parallel algorithms for matrix multiplication.

## 1. Sequential Optimizations
> [!info]
> - **Standard (ijk):** $O(n^3)$ operations. Suffers from **stride-n** bottleneck in Matrix B.
> - **Reordered (ikj):** Sequential access to B and C; improves spatial locality.
> - **Tiling (Blocking):** Divides into $b \times b$ submatrices.
>     - **Comm. Complexity:** $O(n^3 / \sqrt{M})$ where $M$ is cache size.

## 2. Distributed Algorithms
| Algorithm | Partitioning | Comm. Ratio | Notes |
| :--- | :--- | :--- | :--- |
| **Block-Striped** | 1D Rows | $n/p$ | Limited scalability; logical ring shift. |
| **Cannon's** | 2D Checkerboard | $n/\sqrt{p}$ | Requires initial **skewing** (alignment). |
| **Fox's** | 2D Checkerboard | $n/\sqrt{p}$ | Row-broadcast and column-shift. |
| **SUMMA** | 2D Checkerboard | $n/\sqrt{p}$ | Panel-based; supports **pipelining**. |

## 3. Strassen's Algorithm
> [!equation] Complexity
> Reduces $2 \times 2$ multiplications from 8 to 7.
> $$T(n) = O(n^{2.81})$$
> [!warning]
> Only faster for very large $n$ due to $O(n^2)$ addition overhead and numerical instability.

## 4. Scalability Insight
> [!info]
> 2D Checkerboard decomposition is superior to 1D Striped because it minimizes the total data movement ($O(n^2/\sqrt{p})$ vs $O(n^2)$), allowing the **Arithmetic Intensity** to remain higher as $p$ increases.
