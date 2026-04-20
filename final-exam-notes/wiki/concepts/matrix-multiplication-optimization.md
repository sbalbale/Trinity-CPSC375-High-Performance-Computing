---
aliases: [Loop Reordering, ikj Matrix Multiplication, Matrix Multiplication, Tiling Block Matrix]
tags: [performance, matrix, cache]
sources: [lec30.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Matrix Multiplication Optimization

> [!abstract]
> **Matrix Multiplication Optimization** refers to techniques like **Loop Reordering** and **Tiling** used to improve sequential performance. These methods focus on maximizing **cache locality** to overcome the "memory wall" where memory bandwidth limits CPU throughput.

## Core Mechanics

### Loop Reordering (ikj)
The standard $O(n^3)$ algorithm is the `ijk` version. By reordering the loops to `ikj`, the inner loop on `j` accesses `C[i][j]` and `B[k][j]` sequentially.
- **Benefit**: Spatial locality is greatly improved, as processors can fully utilize each loaded cache line.

### Tiling (Blocking)
Divides matrices into $b \times b$ tiles (blocks) that fit within the L1 or L2 cache.
- **Benefit**: Each element of a tile is loaded into the cache and reused $b$ times before being evicted.
- **Trade-off**: Increases the number of loops (from 3 to 6) but significantly reduces memory traffic.

> [!warning] Stride Bottleneck
> In the `ijk` algorithm, Matrix B is accessed with a **stride of $n$**. If $n$ is larger than the number of cache lines, every access to B results in a cache miss, causing severe performance degradation.

## Implementations & Examples

> [!code] Tiling Logic
> ```c
> for (ii = 0; ii < n; ii += block_size)
>   for (jj = 0; jj < n; jj += block_size)
>     for (kk = 0; kk < n; kk += block_size)
>       // standard mult on blocks [ii, ii+b] x [kk, kk+b]
> ```

## Connections
* **Prerequisites:** [[spatial-locality]].
* **Used In:** BLAS (Basic Linear Algebra Subprograms) libraries.
* **Contrasts With:** [[strassens-algorithm]] (algorithmic vs architectural optimization).
