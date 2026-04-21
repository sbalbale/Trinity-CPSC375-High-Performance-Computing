---
tags: [homework, mpi, distributed-memory, spmd, communication]
sources: [Homework 20.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 20

**Source:** Homework 20.pdf
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment provides a comprehensive review of **Distributed-Memory Systems** and the **MPI** standard. It explores the architectural differences between shared and distributed memory, the lifecycle of MPI processes, the mechanics of point-to-point communication (matching, status, and counts), and the performance/correctness implications of blocking communication.

## Key Claims

- Distributed-memory systems require explicit message passing, which eliminates memory bus contention but increases programming complexity.
- MPI programs follow the **SPMD** model, where every process runs the same binary but identifies itself via a unique **rank**.
- `MPI_Init` and `MPI_Finalize` are mandatory and must encapsulate all other MPI calls to ensure a valid execution environment.
- Message matching is strict, requiring agreement on the communicator, rank, and tag.
- Blocking communication can lead to **deadlocks** if not carefully ordered, especially when implementation-specific buffers are exceeded.

## Entities Mentioned

- [[mpi]] — Message Passing Interface.

## Concepts Covered

- [[distributed-memory-model]] — Architecture and trade-offs.
- [[shared-memory-model]] — Comparison with distributed.
- [[spmd]] — Execution paradigm.
- [[communicators-mpi]] — `MPI_COMM_WORLD`.
- [[point-to-point-communication]] — `MPI_Send` and `MPI_Recv`.
- [[mpi-status]] — Handling metadata.
- [[deadlock]] — Communication hazards.
