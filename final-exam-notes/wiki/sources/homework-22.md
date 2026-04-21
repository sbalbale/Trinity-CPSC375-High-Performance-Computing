---
tags: [homework, mpi, derived-types, performance, synchronization]
sources: [Homework 22.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 22

**Source:** Homework 22.pdf
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment covers advanced MPI features for heterogeneous data communication and accurate performance measurement. It requires defining custom datatypes using `MPI_Type_create_struct` and explains the importance of memory addresses for portability. It also analyzes a standard timing pattern using `MPI_Barrier` and `MPI_Reduce` to calculate parallel execution time.

## Key Claims

- **Derived datatypes** allow MPI to bundle different variables into a single communication operation, reducing latency.
- `MPI_Get_address` is required because compilers often insert **padding** between members of a struct, making simple size calculations unreliable for cross-platform parallel code.
- A **barrier** before timing ensures that no process starts the computation until all processes are ready, providing a true measure of the parallel work.
- The **MAX reduction** is the correct way to measure parallel time because the total duration of a parallel block is determined by its slowest-running process.

## Entities Mentioned

- [[mpi]] — Message Passing Interface.

## Concepts Covered

- [[mpi-derived-datatypes]] — `MPI_Type_create_struct` and displacements.
- [[mpi-barrier]] — Synchronization for timing.
- [[performance-measurement-mpi]] — `MPI_Wtime` and `MPI_MAX` reduction.
- [[portability-mpi]] — Why manual memory mapping is necessary.
