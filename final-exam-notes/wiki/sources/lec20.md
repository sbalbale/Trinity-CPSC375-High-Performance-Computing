---
tags: [mpi, distributed-memory, programming, spmd]
sources: [lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Introduction to MPI

**Source:** lec20.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture introduces the fundamentals of **Distributed-Memory Programming** using the **Message Passing Interface (MPI)**. It covers the difference between shared and distributed memory, the basic structure of an MPI program, and the mechanics of point-to-point communication.

## Key Claims

- **MPI** is the dominant standard for message-passing, providing portability and control over the memory hierarchy.
- Programs are typically written in an **SPMD** (Single Program, Multiple Data) style, where each process executes the same code but operates on different data based on its **rank**.
- `MPI_COMM_WORLD` is the default communicator containing all processes.
- **Process 0** is conventionally used for handling user input (`stdin`) and coordinating results, as most MPI implementations restrict `scanf` to a single process.
- **Blocking**: `MPI_Recv` always blocks until a matching message is received, while `MPI_Send` behavior depends on buffer sizes and implementation.

## Entities Mentioned

- [[mpi]] — Message Passing Interface standard.

## Concepts Covered

- [[distributed-memory-model]] — Memory private to each process.
- [[spmd]] — Single Program, Multiple Data.
- [[point-to-point-communication]] — Basic `MPI_Send` and `MPI_Recv`.
- [[trapezoidal-rule-mpi]] — Example of parallel numerical integration.
- [[communicators-mpi]] — Logical groups of processes.
- [[mpi-status]] — Handling unknown message metadata.
