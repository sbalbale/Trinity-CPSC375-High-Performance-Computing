---
tags: [performance, scalability, sorting]
sources: [lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Isoefficiency and Batcher's Sorting

**Source:** lec28.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture introduces the **Isoefficiency Function** as a formal metric for scalability, explaining how problem size must grow relative to processors to maintain efficiency. It also covers **Batcher's parallel sorting algorithms**, specifically **Odd-Even Mergesort** and **Bitonic Mergesort**, which are highly regular and efficient for parallel architectures.

## Key Claims

- **Isoefficiency** relates work $W$ to overhead $T_o$: $W \propto T_o$ to keep efficiency $E$ constant.
- Algorithms with a linear isoefficiency function $O(p)$ are the most scalable.
- **Odd-Even Mergesort** uses a recursive interleaving of odd and even indices to merge sorted lists.
- **Bitonic Mergesort** converts an unordered list into a large bitonic sequence through successive phases, eventually sorting it in $O(\log^2 n)$ parallel time.
- A **Bitonic Sequence** can be split into two smaller bitonic sequences by comparing $a_i$ and $a_{i+n/2}$, a property that forms the basis of the bitonic sort.

## Entities Mentioned

- [[mpi]] — Context for communication overhead in isoefficiency.

## Concepts Covered

- [[isoefficiency-function]] — Scaling work to maintain efficiency.
- [[odd-even-mergesort]] — Recursive interleaving merge.
- [[bitonic-sequence]] — Sequences with at most one local maximum and minimum.
- [[bitonic-mergesort]] — $O(\log^2 n)$ parallel sorting algorithm.
- [[batchers-sorting-networks]] — Hardware-inspired parallel sorting.
