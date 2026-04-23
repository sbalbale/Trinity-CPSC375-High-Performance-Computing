---
tags: [performance, algorithm, matrix, cache]
sources: [lec30.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Matrix Multiplication I

**Source:** lec30.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture focuses on the performance challenges of standard matrix multiplication and the optimizations used to overcome cache bottlenecks. It details **Loop Reordering** (ikj version), **Tiling (Blocking)** for explicit cache management, and **Strassen's Algorithm** for reducing the algebraic complexity of the operation.

## Key Claims

- The standard `ijk` matrix multiplication has poor spatial locality for Matrix B, leading to $O(n^3)$ memory transfers.
- **Loop Reordering** to `ikj` significantly improves performance by accessing elements sequentially, but does not explicitly manage cache capacity.
- **Tiling** reduces the communication complexity to $O(n^3 / \sqrt{M})$, where $M$ is the cache size, by keeping submatrices in the cache for repeated use.
- **Strassen's Algorithm** achieves $O(n^{2.81})$ complexity by using 7 multiplications instead of 8 for a $2 \times 2$ block, though it introduces more additions.

## Entities Mentioned

- [[strassen]] — Creator of the $O(n^{2.81})$ matrix multiplication algorithm.

## Concepts Covered

- [[matrix-multiplication-optimization]] — Loop reordering (ikj) and tiling.
- [[tiling-block-matrix]] — Explicit cache management via submatrices.
- [[strassens-algorithm]] — Divide-and-conquer $O(n^{2.81})$ multiplication.
- [[spatial-locality]] — Sequential memory access patterns.
- [[communication-complexity]] — The cost of data movement between memory levels.
