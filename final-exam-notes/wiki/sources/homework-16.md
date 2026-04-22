---
tags: [homework, openmp, race-condition, scoping]
sources: [Homework 16.pdf, HW16_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 16

**Source:** Homework 16.pdf, HW16_Solution.md
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment focuses on **OpenMP variable scoping** and the mechanics of **Race Conditions**. It requires analyzing a C code segment to identify shared and private variables and explaining why certain shared updates result in non-deterministic behavior. It also includes a practical programming exercise to use basic OpenMP runtime functions.

## Key Claims

- Variables declared outside a parallel region and not explicitly marked private are **shared** by default.
- Variables declared inside a parallel region are **private** to each thread.
- Concurrent updates to shared variables without synchronization (like `b = b + a`) lead to **race conditions**.

## Entities Mentioned

- [[openmp]] — The parallel API used.

## Concepts Covered

- [[data-sharing-clauses]] — Specifically `private` and implicit `shared`.
- [[race-condition]] — Analysis of concurrent updates.
- [[parallel-region]] — Defining code blocks for threads.
- [[omp-runtime-functions]] — `omp_get_thread_num`, `omp_get_num_threads`.
