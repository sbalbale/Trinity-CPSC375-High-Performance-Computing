---
aliases: [Lecture 30 Summary, Matrix Multiplication I Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec30.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 30: Matrix Multiplication I

> [!abstract]
> This lecture discusses the sequential performance optimizations for matrix multiplication, focusing on **Cache Locality** and **Algorithmic Complexity**. It introduces Loop Reordering, Tiling (Blocking), and Strassen's Algorithm.

## The Cache Problem
Standard matrix multiplication (the `ijk` version) has $O(n^3)$ work complexity. However, as the matrix size $n$ increases, Matrix B becomes too large for the cache.
- **Problem**: Accessing `B[k][j]` with the inner loop on `k` results in a **stride-n** access pattern.
- **Consequence**: This causes frequent cache misses, making memory transfers the primary bottleneck.

## Optimization Techniques

### 1. Loop Reordering (ikj Version)
Changing the loop order from `ijk` to `ikj` improves spatial locality.
> [!code] ikj Implementation
> ```c
> for (i = 0; i < n; i++) {
>     for (k = 0; k < n; k++) {
>         r = a[i][k]; // Reused n times
>         for (j = 0; j < n; j++)
>             c[i][j] += r * b[k][j]; // Sequential access
>     }
> }
> ```
This access pattern uses adjacent memory locations, maximizing cache line utilization.

### 2. Tiling (Block Matrix Multiplication)
Tiling divides matrices into $b \times b$ submatrices (tiles) that are small enough to fit in the cache.
- **Benefit**: Elements of each submatrix are reused multiple times while they are still in the cache.
- **Communication Complexity**: Reduced from $O(n^3)$ to $O(n^3 / \sqrt{M})$, where $M$ is the cache size.

### 3. Strassen's Algorithm
A divide-and-conquer approach that reduces the number of multiplications.
- **Standard $2 \times 2$**: 8 multiplications, 4 additions.
- **Strassen's $2 \times 2$**: 7 multiplications, 18 additions.
> [!equation] Time Complexity
> $$T(n) = 7 T(n/2) + O(n^2) = O(n^{\log_2 7}) \approx O(n^{2.81})$$
For very large matrices, the reduction in multiplications (which are more expensive than additions) results in a faster runtime.
