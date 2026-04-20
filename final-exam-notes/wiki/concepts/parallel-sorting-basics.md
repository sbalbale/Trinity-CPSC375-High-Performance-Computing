---
aliases: [Sorting Primitives, Compare-and-Exchange Basics]
tags: [algorithm, sorting, basics]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Sorting Basics

> [!abstract]
> **Parallel Sorting Basics** encompass the fundamental operations used to build scalable parallel sorting algorithms. The two most critical primitives are **Compare-and-Exchange** (for single values) and **Merge-and-Split** (for sorted lists).

## Core Primitives

### 1. Compare-and-Exchange
- The basic operation where two values are compared and swapped if out of order.
- In distributed memory, this involves message passing between two ranks.

### 2. Merge-and-Split
- Used when each process holds multiple elements ($n > p$).
- Processes exchange their sorted local lists, merge them, and each keeps one "half" of the result.
- This maintains the global sorted property as the algorithm progresses.

## Complexity
- The theoretical goal is $O\left( \frac{n \log n}{p} \right)$ total parallel time.

## Connections
* **Used In:** [[odd-even-transposition-sort]], [[parallel-mergesort]], [[bitonic-mergesort]].
* **Prerequisites:** [[mpi]], [[spmd]].
