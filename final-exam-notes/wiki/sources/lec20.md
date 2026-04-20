---
tags: [openmp, shared-memory, mpi, distributed-memory, parallel-programming]
sources: [lec20.pdf, lec20.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Lecture 20: Parallel Programming Models (OpenMP & MPI)

**Source:** lec20.pdf, lec20.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Slides / Text

## Summary

This lecture provides a foundational introduction to both **OpenMP** (shared-memory) and **MPI** (distributed-memory) parallel programming models. It contrasts the two architectures, introduces the **Fork-Join** and **SPMD** models, and covers basic syntax for both.

## Key Claims

- OpenMP is for **Shared Memory** systems using directives; MPI is for **Distributed Memory** using library calls.
- **Fork-Join Model** is the core execution mechanism for OpenMP.
- **SPMD (Single-Program Multiple-Data)** is the core pattern for MPI programs.
- MPI programs require explicit communication (Send/Recv), while OpenMP threads communicate implicitly via shared variables.

## Entities Mentioned

- [[openmp-architecture-review-board]] — Maintains OpenMP standard.
- [[MPI]] — The Message Passing Interface standard.

## Concepts Covered

- [[shared-memory-model]] — Communication through shared variables.
- [[distributed-memory-model]] — Each process has its own address space.
- [[fork-join-model]] — Thread creation and synchronization pattern.
- [[spmd]] — One program running on multiple data sets.
- [[omp-parallel-directive]] — Defining parallel regions in OpenMP.
- [[mpi-basics]] — Init, Finalize, Rank, and Size.
