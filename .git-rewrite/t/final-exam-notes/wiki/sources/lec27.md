---
tags: [performance, scalability, metrics]
sources: [lec27.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Performance Analysis and Scalability

**Source:** lec27.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture provides a comprehensive overview of parallel performance analysis. It defines key metrics like **Speedup** and **Efficiency**, and compares the pessimistic and optimistic views of scaling provided by **Amdahl's Law** and **Gustafson's Law**. It also introduces the **Karp-Flatt Metric** for diagnosing scaling bottlenecks and distinguishes between **Strong** and **Weak Scalability**.

## Key Claims

- **Linear speedup** ($S_p = p$) is the "Gold Standard" but rarely achieved due to overhead.
- **Amdahl's Law** shows that a small serial fraction $f$ creates a hard ceiling on speedup, regardless of the number of processors.
- **Gustafson's Law** argues that we should scale the problem size with processing power, leading to much better "scaled speedup."
- **Karp-Flatt Metric** ($e$) allows empirical diagnosis: constant $e$ means algorithmic serial limits, while increasing $e$ means implementation/overhead limits.
- **Strong Scalability** measures time reduction for a fixed problem, while **Weak Scalability** measures efficiency maintenance for a growing problem.

## Entities Mentioned

- [[mpi]] — Referenced in the context of parallel overhead.

## Concepts Covered

- [[parallel-speedup]] — Ratio of sequential to parallel time.
- [[parallel-efficiency]] — Speedup per processor.
- [[amdahls-law]] — Fixed-size speedup limits.
- [[gustafsons-law]] — Scaled-size speedup perspective.
- [[karp-flatt-metric]] — Experimental serial fraction.
- [[strong-scalability]] — Fixed workload scaling.
- [[weak-scalability]] — Proportional workload scaling.
