---
tags: [#source/lecture, #exam/mpi, #exam/collective-communication]
sources: [lec21.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Collective Communication

**Source:** lec21.pdf
**Date ingested:** 2026-04-20
**Type:** lecture_slides

## Summary

This lecture introduces MPI Collective Communication patterns, including Reduce, Allreduce, Broadcast, Scatter, Gather, and Allgather. It contrasts collective communication with point-to-point communication and explains data partitioning strategies for parallel vector and matrix operations.

## Key Claims

- All processes in a communicator must call the same collective function.
- Collective communications don't use tags; they match solely based on the communicator and the order they are called.
- Data partitioning (Block, Cyclic, Block-cyclic) determines how data is distributed across processes.

## Entities Mentioned

- [[MPI]] — Message Passing Interface, used for distributed memory parallelism.

## Concepts Covered

- [[Collective Communication]] — Communication involving all processes in a communicator.
- [[MPI_Reduce]] — Combines values from all processes using an operation and delivers the result to one process.
- [[MPI_Allreduce]] — Like Reduce, but delivers the result to all processes.
- [[MPI_Bcast]] — Broadcasts data from one process to all other processes.
- [[MPI_Scatter]] — Distributes distinct chunks of an array from one process to all processes.
- [[MPI_Gather]] — Collects chunks of an array from all processes into a single array on one process.
- [[MPI_Allgather]] — Collects chunks from all processes and distributes the complete array to all processes.
- [[Data Partitioning]] — Strategies (Block, Cyclic, Block-cyclic) for dividing data among processes.
