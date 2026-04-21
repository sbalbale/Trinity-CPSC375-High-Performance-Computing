---
tags: [homework, mpi, algorithm, sorting, complexity]
sources: [Homework 25.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 25

**Source:** Homework 25.pdf
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment covers foundational parallel sorting algorithms, specifically **Odd-Even Transposition Sort** and **Mergesort**. It requires a deep analysis of algorithm mechanics, time complexity under the assumption of unlimited processors, and the impact of synchronization overhead on parallel performance. It also involves designing distributed MPI implementations for both sorting methods.

## Key Claims

- **Odd-Even Transposition Sort** is highly parallel but limited by a high number of synchronization phases ($n$ phases for $n$ elements).
- **Parallel Mergesort** is work-efficient ($O(n \log n)$) and achieves better parallel time complexity than simple comparison sorts.
- Synchronization between phases is mandatory to maintain data consistency, but it introduces a performance bottleneck that grows with the number of processors and phases.

## Entities Mentioned

- [[mpi]] — Message Passing Interface.

## Concepts Covered

- [[odd-even-transposition-sort]] — Mechanics and phases.
- [[parallel-mergesort]] — Complexity and tree-based merging.
- [[synchronization-overhead]] — Impact on parallel sorting.
- [[work-efficiency]] — Comparing sequential and parallel work.
- [[spmd]] — Implementation model for distributed sorting.
