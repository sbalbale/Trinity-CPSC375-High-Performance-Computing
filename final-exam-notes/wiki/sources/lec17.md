---
tags: [parallel-performance, amdahls-law, scalability]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Lecture 17: Performance Analysis of Parallel Algorithms

**Source:** lec17.pdf
**Date ingested:** 2025-05-15
**Type:** Lecture Slides

## Summary

This lecture introduces the quantitative metrics used to evaluate the performance and scalability of parallel algorithms. It covers fundamental laws like Amdahl's and Gustafson's and discusses the factors that limit parallel performance.

## Key Claims

- **Speedup** measures how much faster the parallel version is compared to the sequential version.
- **Amdahl's Law** suggests that the serial portion of an algorithm creates a hard ceiling for speedup, regardless of the number of processors.
- **Gustafson's Law** provides a more optimistic view by considering scaled workloads (increasing problem size with more processors).
- **Efficiency** measures the fraction of time for which a processor is usefully employed.

## Entities Mentioned

- [[gene-amdahl]] — Computer architect known for Amdahl's Law.
- [[john-gustafson]] — Computer scientist known for Gustafson's Law.

## Concepts Covered

- [[parallel-speedup]] — Definition and calculation.
- [[parallel-efficiency]] — Metric for processor utilization.
- [[amdahls-law]] — Fixed-size speedup limitations.
- [[gustafsons-law]] — Scaled-size speedup perspective.
- [[isoefficiency-function]] — Scalability analysis metric.
- [[overhead-parallel-computing]] — Sources of performance loss (communication, synchronization, idle time).
