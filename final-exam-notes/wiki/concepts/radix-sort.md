---
aliases: [LSD Sort, Base Sort]
tags: [algorithm, sorting, mpi, radix]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Radix Sort

> [!abstract]
> **Radix Sort** is a non-comparison sorting algorithm that processes digits of numbers individually, typically starting from the Least Significant Digit (LSD). It relies on a **stable** sorting primitive (like [[counting-sort]]) to maintain the relative order of elements with the same digit.

## Core Mechanics

### The Algorithm (LSD)
1. **Represent**: Numbers are seen in positional digit representation (e.g., base $b = 10$ or $b = 256$).
2. **Sort by Digits**:
    - Pass 1: Sort by the units digit ($1s$).
    - Pass 2: Sort by the tens digit ($10s$).
    - ...
    - Pass $d$: Sort by the most significant digit.
3. **Requirement**: The sorting algorithm used for each pass **must be stable**.

> [!equation] Time Complexity
> - Total Time: $O(d(n + b))$
> - $d$: Number of digits (passes).
> - $n$: Number of elements.
> - $b$: Base of the number system.

> [!warning] Parallel Challenges
> Parallelizing radix sort in distributed memory requires global coordination for the prefix sums of each bucket. If stability is not maintained across processor boundaries, the sort will fail.

## Implementations & Examples

> [!example] Distributed Parallel Radix Sort
> To sort 32-bit unsigned integers with $b=256$ (one byte per digit):
> 1. Each process counts local frequencies for a byte.
> 2. `MPI_Exscan` is used to find the global offsets for each bucket across all processes.
> 3. `MPI_Alltoallv` is used to send the elements to their new destination node.

## Connections
* **Prerequisites:** [[counting-sort]].
* **Used In:** Sorting large datasets of integers or keys (Assignment 8).
* **Contrasts With:** [[compare-and-exchange]] sorting algorithms ($O(n \log n)$).
