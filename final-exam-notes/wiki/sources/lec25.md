---
tags: [mpi, algorithm, sorting]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Sorting Algorithms I

**Source:** lec25.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture introduces the fundamentals of parallel sorting, starting with the **compare-and-exchange** primitive. it covers the parallelization of simple algorithms like **Bubble Sort** and **Odd-Even Transposition Sort**, as well as the high-level parallel strategies for **Mergesort** and **Quicksort** using tree-based process allocation.

## Key Claims

- The theoretical best speedup for a sorting algorithm using $p$ processors is $O((n \log n) / p)$.
- In message-passing, a **compare-and-exchange** can be implemented by one process doing the comparison or both doing it to reduce communication turns.
- When $n > p$, processes perform a **Merge-and-Split** operation: they exchange their sorted local lists, merge them, and each keeps either the smaller or larger half.
- **Odd-Even Transposition Sort** is a highly regular parallel sorting algorithm suitable for $O(p)$ steps on $p$ processors.
- **SPMD Consistency**: Duplicating comparisons on different processors can lead to diverging execution paths if floating-point precision varies between hardware.

## Entities Mentioned

- [[mpi]] — Used for implementing compare-and-exchange and merge-split operations.

## Concepts Covered

- [[compare-and-exchange]] — The fundamental building block of parallel sorting.
- [[merge-and-split]] — Scaling sorting when $n > p$.
- [[odd-even-transposition-sort]] — A parallel variant of bubble sort.
- [[parallel-mergesort]] — Divide-and-conquer sorting using process trees.
- [[parallel-quicksort]] — Pivot-based sorting in a parallel environment.
