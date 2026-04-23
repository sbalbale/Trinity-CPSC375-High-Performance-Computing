---
tags: [exam, study-guide, course_hpc]
sources: [exam2_study_guide.html]
created: 2026-04-20
updated: 2026-04-20
---

# Exam 2 Study Guide

**Source:** exam2_study_guide.html
**Date ingested:** 2026-04-20
**Type:** study_guide

## Summary
A comprehensive study guide for CPSC 375 Exam 2 (Lectures 15-31). It covers parallel algorithm design (Foster's), OpenMP, threads, sockets, MPI, the Sieve of Eratosthenes, parallel sorting, performance analysis, and parallel matrix multiplication.

## Key Claims
- **Exam Structure**: Divided into **closed-notes** (multiple-choice on terminology/concepts) and **open-notes** (short-answer on homework/assignment exercises).
- **Foster's Methodology**: Stages are partition → communicate → agglomerate → map (PCAM).
- **Optimization Priority**: Reduction and atomics are preferred over critical sections for scalability.
- **Scaling Philosophy**: Gustafson's Law is optimistic for scientific simulations because problem size scales with processor count.

## Entities Mentioned
- [[openmp]] — Compiler directives and runtime library.
- [[mpi]] — Message Passing Interface for distributed systems.
- [[slurm]] — Resource manager for job submission.
- [[pine-cluster]] — Target hardware for assignments.

## Concepts Covered
- [[race-condition]] — Predictability issues in concurrent code.
- [[amdahls-law]] — Fixed-size scaling (pessimistic).
- [[gustafsons-law]] — Scaled-size scaling (optimistic).
- [[karp-flatt-metric]] — Experimental overhead diagnosis.
- [[shearsort]] — 2D mesh sorting algorithm.
- [[cannons-algorithm]] — Checkerboard matrix multiplication.
- [[foxs-algorithm]] — Outer-product based matrix multiplication.
- [[relational-algebra]] — Foundational database math.
