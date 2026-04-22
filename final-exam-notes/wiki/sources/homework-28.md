---
tags: [homework, performance, scalability, sorting, bitonic-sort]
sources: [Homework 28.pdf, HW28_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 28

**Source:** Homework 28.pdf, HW28_Solution.md
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment focuses on the **Isoefficiency Function** and the **Bitonic Sorting** algorithm. It requires a formal derivation of the isoefficiency metric to quantify algorithm scalability and an analysis of a specific parallel algorithm's growth requirements. It also includes a detailed trace of the Bitonic Sorting algorithm on an unordered sequence of 8 integers, exploring the bitonic split property.

## Key Claims

- The **Isoefficiency Function** $W = K \cdot p T_o$ defines the necessary relationship between problem size and hardware scale to maintain steady efficiency.
- Algorithms with high-power polynomial growth requirements (e.g., $O(p^4)$) are considered poorly scalable in practical HPC environments.
- **Bitonic Mergesort** sorts through a series of phases that convert unordered data into bitonic sequences of doubling lengths, ultimately resulting in a monotonic sorted list.
- A single bitonic split pass on a sequence of length $n$ results in two smaller bitonic sequences of length $n/2$ that are globally ordered relative to each other.

## Entities Mentioned

- [[mpi]] — Context for communication overhead.

## Concepts Covered

- [[isoefficiency-function]] — Derivation and scalability classes.
- [[bitonic-mergesort]] — Phase-by-phase execution.
- [[bitonic-sequence]] — Definition and split property.
- [[communication-overhead]] — Impact on scalability.
- [[compare-and-exchange]] — Counting operations in sorting networks.
