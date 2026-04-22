---
tags: [homework, openmp, atomic, scheduling, sections]
sources: [Homework 19.pdf, HW19_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 19

**Source:** Homework 19.pdf, HW19_Solution.md
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment explores advanced thread coordination and optimization techniques in **OpenMP**. It includes empirical performance comparisons between `atomic` and `critical` directives, a demonstration of **load balancing** through dynamic scheduling in irregular loops, and an introduction to functional parallelism using **parallel sections**.

## Key Claims

- **`atomic`** is faster than `critical` for simple updates because it leverages hardware-level atomic operations instead of general-purpose mutual exclusion locks.
- **Dynamic scheduling** resolves load imbalance in loops where iterations have unequal execution times by assigning work to threads at runtime as they become available.
- OpenMP **sections** can execute independent tasks concurrently, but threads without an assigned section remain idle during the construct's execution.

## Entities Mentioned

- [[openmp]] — The parallel API used.

## Concepts Covered

- [[omp-synchronization]] — Atomic vs. Critical.
- [[omp-scheduling]] — Static vs. Dynamic.
- [[omp-tasks-sections]] — Specifically the `sections` directive.
- [[load-balancing]] — Fixing execution time skew.
- [[performance-measurement-openmp]] — Using `omp_get_wtime()`.
