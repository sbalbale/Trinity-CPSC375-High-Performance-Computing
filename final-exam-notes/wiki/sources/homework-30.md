---
tags: [#source, #exam/theory, #homework/solutions]
sources: [Homework 30.pdf, HW30_Solution.md]
created: 2026-04-21
updated: 2026-04-21
aliases: [HW30 Source]
---

# Homework 30

**Source:** Homework 30.pdf
**Date ingested:** 2026-04-21
**Type:** homework

## Summary

This homework focuses on optimizing matrix multiplication performance through cache management techniques, including loop reordering ($ikj$) for spatial locality, tiling (blocking) for temporal locality, and Strassen's divide-and-conquer algorithm for reduced computational complexity.

## Key Claims

- The standard $ijk$ matrix multiplication suffers from cache capacity misses due to large strides when accessing matrix B.
- Reordering loops to $ikj$ maximizes spatial locality by ensuring sequential row-major access for both matrices B and C.
- Tiling (blocking) improves temporal locality by keeping sub-matrices in cache, reducing communication complexity to $O(n^3 / \sqrt{M})$.
- Strassen's algorithm reduces the number of recursive multiplications for a $2 \times 2$ block from 8 to 7, improving complexity to $O(n^{2.81})$.

## Entities Mentioned

- [[Strassen's Algorithm]] — divide-and-conquer approach to matrix multiplication.

## Concepts Covered

- [[Matrix Multiplication Optimization]] — techniques to improve cache performance.
- [[Spatial Locality]] — improved via loop reordering ($ikj$).
- [[Temporal Locality]] — improved via tiling (blocking).
- [[Tiling]] — blocking strategy for cache management.