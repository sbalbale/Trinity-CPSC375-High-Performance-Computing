---
aliases: [Tree-Based Quicksort, Pivot Sort]
tags: [algorithm, sorting, mpi, divide-and-conquer]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Quicksort

> [!abstract]
> **Parallel Quicksort** is a pivot-based sorting algorithm that uses a tree of processes. A **pivot** is chosen at each stage to partition the data into two sublists, which are then processed by different sub-trees of processors.

## Core Mechanics

### Phases
1. **Pivot Selection (Master)**: A value is chosen (e.g., the first element of the master's list).
2. **Partition (Master)**: The master process divides its list into two sublists: elements smaller than or equal to the pivot, and elements larger than the pivot.
3. **Distribution**: One sublist is sent to another process (the child in the process tree), while the master keeps the other.
4. **Recursion**: Both processes independently repeat the partitioning step using their own pivots and sub-trees of available processes.

> [!equation] Time Complexity
> - Sequential: $O(n \log n)$ average.
> - Parallel: $O(\frac{n}{p} \log \frac{n}{p} + \log^2 p)$ average with $p$ processes.

> [!warning] Load Imbalance
> If the chosen pivot is poor (e.g., always the smallest/largest element), the partitions will be vastly unequal in size, leading to **load imbalance** and $O(n^2)$ worst-case complexity.

## Implementations & Examples

> [!example] Tree Allocation
> A master process (P0) partitions a list and sends the larger half to P4. Then P0 and P4 independently partition their halves and send portions to P2 and P6 respectively. This process continues until all processes have data.

## Connections
* **Prerequisites:** [[mpi]], [[partitioning]].
* **Used In:** Systems where data is initially concentrated on one or a few nodes.
* **Contrasts With:** [[parallel-mergesort]] (which merges from leaves up).
