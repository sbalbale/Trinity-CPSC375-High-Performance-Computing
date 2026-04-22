---
tags: [homework, domain-decomposition, mapping, agglomeration]
sources: [Homework 15.pdf, HW15_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 15

**Source:** Homework 15.pdf, HW15_Solution.md
**Date ingested:** 2026-04-21
**Type:** Homework Assignment & Solutions

## Summary

This assignment covers foundational concepts in parallel algorithm design using a weather forecasting model as a case study. It explores **Domain Decomposition** for 3D grids, the definition of **Primitive Tasks**, and the relationship between task count and processor count for effective **Load Balancing**. It also addresses the trade-offs involved in **Agglomeration** when communication costs are high.

## Key Claims

- Domain decomposition for spatial grids involves partitioning the physical volume into sub-domains.
- A **10:1 ratio** of tasks to processors is a common rule of thumb for effective static load balancing.
- **Agglomeration** reduces communication overhead but can decrease potential parallelism and processor utilization.

## Entities Mentioned

- [[fosters-methodology]] — Framework for parallel design.
- [[lenovo-p340]] — Compute node hardware mentioned.

## Concepts Covered

- [[domain-decomposition]] — Partitioning spatial data.
- [[primitive-task]] — The smallest unit of parallel work.
- [[static-mapping]] — Fixed assignment of tasks.
- [[load-balancing]] — Ensuring even distribution of work.
- [[agglomeration]] — Grouping tasks to reduce overhead.
