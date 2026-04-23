---
aliases: [Loop Reordering, ikj Matrix Multiplication, Matrix Multiplication, Tiling Block Matrix]
tags: [performance, matrix, cache]
sources: [lec30.txt, Homework 30.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Matrix Multiplication Optimization

> [!abstract]
> **Matrix Multiplication Optimization** refers to techniques like **Loop Reordering** and **Tiling** used to improve sequential performance. These methods focus on maximizing **cache locality** to overcome the "memory wall" where memory bandwidth limits CPU throughput.

## Core Mechanics

### Loop Reordering
Reordering the three nested loops can yield 6-10x speedup simply by changing the memory access pattern from **stride-n** to **unit-stride**.

| Order | Time ($n=1000$) | Speedup | Access Pattern (B) |
| :--- | :--- | :--- | :--- |
| **ijk** | 2.5s | 1.0x | Stride-n (Poor) |
| **kij** | 0.4s | 6.25x | Unit-stride (Good) |
| **jik** | 0.25s | **10.0x** | Unit-stride (Best) |

### Tiling (Blocking)
Divides matrices into $b \times b$ tiles (blocks) that fit within the L1 or L2 cache.
- **Data Reuse**: For a $b \times b$ block, elements are loaded once and reused $b$ times.
> [!equation] Arithmetic Intensity
> For tiled multiplication, the ratio of operations to memory accesses is:
> $$\text{Intensity} = \frac{b}{2} \text{ ops/access}$$
> Larger $b$ improves intensity but is limited by the physical cache size $M$.

- **Benefit**: Each element of a tile is loaded into the cache and reused $b$ times before being evicted.
- **Communication Complexity**: Tiling reduces memory transfers from $O(n^3)$ to $O\left( \frac{n^3}{\sqrt{M}} \right)$.

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
