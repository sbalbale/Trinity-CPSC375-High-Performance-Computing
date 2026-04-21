---
tags: [homework, mpi, algorithm, optimization, sieve]
sources: [Homework 24.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 24

**Source:** Homework 24.pdf
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment focuses on two major optimizations for the **Parallel Sieve of Eratosthenes**. The first optimization involves **Odd-Only Storage**, which reduces memory consumption by 50% by omitting even integers. The second optimization involves **Eliminating Broadcast**, where each process independently calculates sieving primes up to $\sqrt{n}$ to remove communication overhead. The assignment requires implementing these changes and benchmarking their performance.

## Key Claims

- Storing only odd numbers in the Sieve array reduces both memory and computational iterations by half.
- Communication latency for broadcasting sieving primes can be a bottleneck in large clusters; redundant local computation is often faster than network transfer.
- Optimized parallel algorithms should prioritize reducing communication-to-computation ratios.

## Entities Mentioned

- [[mpi]] — Message Passing Interface.
- [[sieve-of-eratosthenes]] — The algorithm being optimized.

## Concepts Covered

- [[sieve-optimizations]] — Odd-only and broadcast elimination.
- [[communication-overhead]] — Bottlenecks in `MPI_Bcast`.
- [[local-vs-global-indices]] — Mapping for odd-only arrays.
- [[performance-benchmarking]] — Measuring the impact of optimizations.
