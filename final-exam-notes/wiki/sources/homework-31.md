---
tags: [homework, matrix-multiplication, scalability]
sources: [Homework 31.pdf, HW31_Solution.md]
created: 2026-04-23
updated: 2026-04-23
---

# Homework 31

**Source:** Homework 31.pdf
**Date ingested:** 2026-04-23
**Type:** homework

## Summary

This source contains exercises focused on the performance and scalability of parallel matrix multiplication. It specifically contrasts the computation-to-communication ratios of 1D and 2D decompositions and examines the mechanics of Cannon's, Fox's, and SUMMA algorithms.

## Key Claims

- Cannon's algorithm has a superior computation-to-communication ratio ($n/\sqrt{p}$) compared to block-striped decomposition ($n/p$).
- Cannon's uses in-place shifts while Fox's requires extra memory for broadcasting.
- SUMMA's outer-product approach allows for arbitrary processor grids (not just perfect squares).
- Panel width $b$ in SUMMA must be tuned to balance startup latency and pipelining efficiency.

## Entities Mentioned

- [[pine-cluster]] — Mentioned in context of hardware optimizations.

## Concepts Covered

- [[computation-to-communication-ratio]] — Comparison between 1D and 2D.
- [[cannons-algorithm]] — Initial alignment traces.
- [[foxs-algorithm]] — Memory overhead and hardware optimizations.
- [[summa-algorithm]] — Flexibility and panel width tuning.
- [[checkerboard-decomposition]] — Underlying data distribution.
