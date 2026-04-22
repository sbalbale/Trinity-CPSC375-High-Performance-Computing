---
tags: [homework, performance, speedup, amdahls-law, gustafsons-law, karp-flatt]
sources: [Homework 27.pdf, HW27_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 27

**Source:** Homework 27.pdf, HW27_Solution.md
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment provides a rigorous mathematical treatment of parallel performance metrics. It includes formal proofs for **Amdahl's Law** and **Gustafson's Law**, compares their asymptotic behavior, and requires the derivation of the **Karp-Flatt Metric**. Finally, it uses these tools to analyze benchmarking data from six different programs to diagnose performance bottlenecks and predict future scalability.

## Key Claims

- Amdahl's Law provides a pessimistic view of scaling by fixing the problem size, making the serial fraction a dominant factor as $p$ increases.
- Gustafson's Law provides a more optimistic view by scaling the workload with the hardware, allowing the parallel portion of work to grow.
- The **Karp-Flatt Metric** is the primary tool for distinguishing between algorithmic serial bottlenecks (constant $e$) and implementation overhead bottlenecks (increasing $e$).

## Entities Mentioned

- [[gene-amdahl]] — Author of Amdahl's Law.
- [[john-gustafson]] — Author of Gustafson's Law.

## Concepts Covered

- [[parallel-speedup]] — Formal definition and calculation.
- [[amdahls-law]] — Proof and limits.
- [[gustafsons-law]] — Proof and scaled speedup.
- [[karp-flatt-metric]] — Derivation and diagnostic use.
- [[strong-scalability]] — Relationship to Amdahl.
- [[weak-scalability]] — Relationship to Gustafson.
