---
tags: [homework, data-partitioning, block-decomposition, load-balancing]
sources: [Homework 23.pdf, HW23_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 23

**Source:** Homework 23.pdf, HW23_Solution.md
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment examines the mathematical foundations of **Block Data Decomposition**. It analyzes a "naive" allocation strategy to identify potential for process idleness and compares two balanced decomposition methods. It requires calculating specific data assignments (low/high indices and block sizes) for various combinations of element counts ($n$) and process counts ($p$).

## Key Claims

- Naive block allocation (giving all but the last process $\lceil n/p \rceil$ elements) frequently results in under-utilized or idle processes when $n$ is not a multiple of $p$.
- Balanced decomposition ensures that every process handles either $\lfloor n/p \rfloor$ or $\lceil n/p \rceil$ elements, minimizing load imbalance.
- The formula $L(id) = \lfloor (id \cdot n) / p \rfloor$ effectively distributes the remainder across processes without requiring explicit remainder tracking.

## Entities Mentioned

- [[mpi]] — Context for distributed data arrays.

## Concepts Covered

- [[block-decomposition]] — Method 1 vs. Method 2.
- [[load-balancing]] — Ensuring even data distribution.
- [[local-vs-global-indices]] — Mapping for specific $(n, p)$ pairs.
- [[idle-processes]] — Consequences of poor decomposition.
