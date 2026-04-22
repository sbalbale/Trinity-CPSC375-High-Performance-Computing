---
tags: [homework, mpi, collective-communication, data-partitioning]
sources: [Homework 21.pdf, HW21_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 21

**Source:** Homework 21.pdf, HW21_Solution.md
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment covers **Collective Communication** operations in MPI, including `MPI_Reduce`, `MPI_Bcast`, `MPI_Scatter`, and `MPI_Gather`. It emphasizes the mandatory participation of all processes in collective calls and the importance of parameter consistency. It also explores various **Data Partitioning** strategies (Block, Cyclic, and Block-Cyclic) and their suitability for different workload types.

## Key Claims

- Collective operations are **not** point-to-point; they must be invoked by every process in the communicator to complete.
- The order of collective calls is the only way MPI distinguishes between them; mismatched orders across processes cause incorrect data matching.
- **Cyclic partitioning** is preferred over block partitioning when the computational work per data element varies significantly, as it provides better load balancing.
- Mismatched parameters (like message counts) in collective calls across different processes lead to runtime errors or deadlocks.

## Entities Mentioned

- [[mpi]] — Message Passing Interface.

## Concepts Covered

- [[collective-communication]] — Fundamental principles and participation rules.
- [[data-partitioning]] — Block, Cyclic, and Block-Cyclic assignments.
- [[reduction-operation]] — `MPI_Reduce` vs. `MPI_Allreduce`.
- [[scatter-gather]] — Data distribution and collection mechanics.
- [[load-balancing]] — Choosing partitioning based on work distribution.
