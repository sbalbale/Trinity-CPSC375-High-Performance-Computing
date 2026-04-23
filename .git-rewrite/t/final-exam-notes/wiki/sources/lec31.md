---
tags: [mpi, algorithm, matrix, distributed]
sources: [lec31.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Matrix Multiplication II: Parallel Algorithms

**Source:** lec31.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture details distributed parallel algorithms for matrix multiplication. It contrasts 1D **Block-Striped** partitioning with more scalable 2D **Checkerboard** decompositions. It covers three major checkerboard algorithms: **Cannon's**, **Fox's**, and **SUMMA**, focusing on their communication patterns and computation-to-communication ratios.

## Key Claims

- **Block-Striped** partitioning is limited by a poor computation-to-communication ratio of $n/p$.
- **Cannon's Algorithm** improves the ratio to $n/\sqrt{p}$ but requires a complex "skewing" alignment phase before multiplication begins.
- **Fox's Algorithm** uses broadcasts along rows and shifts along columns, providing a more intuitive outer-product-like execution pattern.
- **SUMMA** is the most versatile and scalable, using rank-$b$ updates with panels and supporting pipelining to hide communication latency.

## Entities Mentioned

- [[mpi]] — Standard library used for implementing these distributed algorithms.

## Concepts Covered

- [[checkerboard-decomposition]] — 2D partitioning of matrices across processors.
- [[cannons-algorithm]] — Initial skewing and shift-multiply.
- [[foxs-algorithm]] — Row-broadcast and column-shift.
- [[summa-algorithm]] — Scalable rank-$b$ panel updates.
- [[computation-to-communication-ratio]] — Key metric for parallel algorithm scalability.
