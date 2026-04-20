---
tags: [#source/lecture, #exam/algorithm_design]
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Algorithm Design

**Source:** lec15.pdf
**Date ingested:** 2026-04-20
**Type:** lecture_slides

## Summary

This lecture covers the **Foster's Design Methodology** (PCAM) for designing parallel algorithms, focusing on Partitioning, Communication, Agglomeration, and Mapping.

## Key Claims

- Designing parallel algorithms requires a structured approach to ensure scalability and efficiency.
- **Foster's Methodology** consists of four distinct stages: Partitioning, Communication, Agglomeration, and Mapping (PCAM).
- Balancing granularity (task size) and communication overhead is crucial for performance.
- Load balancing through Mapping minimizes idle processor time.

## Entities Mentioned

- [[Foster's Methodology]] — A four-step framework for parallel algorithm design (PCAM).

## Concepts Covered

- [[Partitioning]] — Decomposing a problem into small, parallel tasks.
- [[Communication]] — Managing data exchange between tasks.
- [[Agglomeration]] — Combining tasks to reduce communication overhead and improve efficiency.
- [[Mapping (Parallel Programming)]] — Assigning tasks to physical processors to achieve load balance.
- [[Domain Decomposition]] — Partitioning based on data structures.
- [[Functional Decomposition]] — Partitioning based on computations/tasks.
- [[Load Balancing]] — Strategy to distribute work evenly across processors.
