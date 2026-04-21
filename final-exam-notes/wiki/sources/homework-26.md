---
tags: [homework, mpi, synchronization, reduction, point-to-point]
sources: [Homework 26.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 26

**Source:** Homework 26.pdf
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment involves practical MPI programming exercises focusing on two distinct coordination patterns. The first task requires enforcing a strict sequential order of output across distributed processes using point-to-point communication. The second task implements a global summation of random integers generated locally on each process using the `MPI_Reduce` collective.

## Key Claims

- Sequential ordering of asynchronous processes can be achieved by "passing a token" using `MPI_Send` and `MPI_Recv`.
- Random number generation in parallel requires awareness that standard `rand()` may produce the same sequence unless seeds are unique per process.
- `MPI_Reduce` is the standard and most efficient way to aggregate local scalar data into a global result at a root process.

## Entities Mentioned

- [[mpi]] — Message Passing Interface.

## Concepts Covered

- [[point-to-point-communication]] — Used for token-passing synchronization.
- [[reduction-operation]] — Specifically `MPI_SUM`.
- [[determinism-parallel]] — Forcing ordered output in a non-deterministic environment.
- [[mpi-hello-world]] — Reference implementation being modified.
