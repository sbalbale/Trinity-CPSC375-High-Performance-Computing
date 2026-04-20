---
tags: [parallel-design, fosters-methodology, mapping]
sources: [lec16.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Lecture 16: More on Parallel Algorithm Design

**Source:** lec16.pdf
**Date ingested:** 2025-05-15
**Type:** Lecture Slides

## Summary

This lecture delves deeper into the **Mapping** stage of Foster's Methodology, exploring strategies for assigning tasks to processors to balance load and minimize communication. It also covers practical parallelization examples, including Parallel Search and the Sieve of Eratosthenes.

## Key Claims

- **Mapping** is NP-complete for the general case, so heuristics are used.
- The goal of mapping is to minimize total execution time by balancing **load** and minimizing **communication**.
- **Static Mapping** is used when tasks and costs are known a priori; **Dynamic Mapping** is used for unpredictable workloads.
- The **Master-Worker** pattern is a common dynamic mapping strategy.

## Entities Mentioned

- [[fosters-methodology]] — Referenced as the overarching design framework.
- [[sieve-of-eratosthenes]] — A classic algorithm for finding prime numbers, used as a parallelization example.

## Concepts Covered

- [[mapping-parallel-programming]] — In-depth discussion of static vs. dynamic strategies.
- [[static-mapping]] — Assignment at compile-time or start-time.
- [[dynamic-mapping]] — Assignment during execution for load balancing.
- [[task-scheduling]] — The process of deciding which task runs where and when.
- [[master-worker-pattern]] — A specific mapping architecture.
- [[parallel-search]] — Strategies for searching in parallel datasets.
