---
aliases: [Loop Reordering, Blocked Marking]
tags: [performance, cache, parallel-programming]
sources: [lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Cache Optimization in Parallel Programming

> [!abstract]
> **Cache Optimization** aims to increase the **cache hit rate** by processing data in a way that respects spatial and temporal locality. In parallel algorithms, this is often achieved through **loop reordering** or **blocking**.

## Core Mechanics

### Spatial Locality
- Accessing memory locations that are close to each other in time.
- Standard Sieve marks $k, 2k, 3k$, which can be far apart if $k$ is large, causing cache misses.

### Temporal Locality
- Reusing the same data while it is still in the cache.

### Loop Reordering
Exchanging the outer loop (sieving primes) and the inner loop (marking blocks of numbers).
- **Standard**: For each prime $k$, mark all its multiples across the entire array.
- **Optimized**: For each small sub-block of the array, apply all sieving primes up to $\sqrt{n}$ at once.

> [!warning] Calculation Overhead
> The optimized loop requires calculating the first multiple of each prime $k$ for **every sub-block**, which adds some overhead, but this is usually outweighed by the massive cache speedup.

## Implementations & Examples

> [!example] Cache-Friendly Sieve
> If a process has a block of $10^7$ elements and a cache size of $2^{16}$ (64KB), it should divide the $10^7$ elements into small chunks of size $2^{16}$ and mark each chunk fully before moving to the next.

## Connections
* **Prerequisites:** [[sieve-optimizations]], cache concepts.
* **Used In:** Large-scale data processing algorithms.
* **Contrasts With:** Naive element-by-element processing.
