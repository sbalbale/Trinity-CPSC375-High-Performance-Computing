---
tags: [mpi, algorithm, sieve, decomposition]
sources: [lec23.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Sieve of Eratosthenes

**Source:** lec23.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture details the parallelization of the **Sieve of Eratosthenes** using **domain decomposition**. It contrasts **interleaved (cyclic)** and **block decomposition** strategies, providing mathematical methods and macros for implementing block decomposition in MPI. The lecture also covers the implementation details of the parallel algorithm, including the use of `MPI_Bcast` and performance analysis.

## Key Claims

- **Interleaved decomposition** leads to load imbalance in the Sieve algorithm because the density of primes decreases as $n$ increases, and smaller primes have more multiples to mark.
- **Block decomposition** is preferred for load balancing.
- **Method #2 for Block Decomposition** (using $\lfloor in/p \rfloor$) is often preferred for its simplicity in scattering larger blocks.
- The assumption $p < \sqrt{n}$ is critical for ensuring process 0 always contains the next sieving prime.
- The parallel complexity is dominated by the marking time divided by $p$, plus the log-based communication overhead of broadcasting sieving primes.

## Entities Mentioned

- [[sieve-of-eratosthenes]] — The primary algorithm being parallelized.
- [[mpi]] — Used for implementation (specifically `MPI_Bcast`).

## Concepts Covered

- [[block-decomposition]] — Strategy for dividing data into contiguous chunks.
- [[interleaved-decomposition]] — Cyclic data distribution.
- [[load-balancing]] — Specifically why cyclic distribution fails for Sieve.
- [[local-vs-global-indices]] — Mapping indices in parallel arrays.
- [[performance-analysis-sieve]] — Modeling execution time with communication overhead.
