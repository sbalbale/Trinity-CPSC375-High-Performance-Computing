---
aliases: [Tree-Based Mergesort, Divide-and-Conquer Sort]
tags: [algorithm, sorting, mpi, divide-and-conquer]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Mergesort

> [!abstract]
> **Parallel Mergesort** is a divide-and-conquer sorting algorithm that uses a **tree of processes** to distribute the sorting workload. The list is recursively divided down the tree, sorted at the leaves, and merged back up.

## Core Mechanics

### Phases
1. **Divide (Master Process)**: The initial unsorted list is divided in half and sent to two child processes. This continues until single elements (or local sublists) are reached at the **leaf nodes**.
2. **Local Sort**: Each leaf process sorts its portion of the data (often sequentially).
3. **Merge**: Pairs of sorted sublists are sent back up the tree to parent processes, which merge them into a single sorted list.

> [!equation] Time Complexity
> - Sequential: $O(n \log n)$
> - Parallel: $O(\frac{n}{p} \log \frac{n}{p} + \frac{n}{p} \log p)$ with $p$ processes.

> [!warning] Process Idle Time
> In a tree-based mergesort, higher-level processes are idle while lower-level ones sort, and vice-versa during the merge phase. This can limit the efficiency of the parallelization.

## Implementations & Examples

> [!example] Tree Allocation
> A list of 16 numbers is distributed among 8 processes in a binary tree. 
> - Level 0: Process 0 (Root)
> - Level 1: Processes 0, 4
> - Level 2: Processes 0, 2, 4, 6
> - Level 3 (Leaves): Processes 0-7

## Connections
* **Prerequisites:** [[mpi]], [[merge-and-split]].
* **Used In:** Systems with tree-like network topologies.
* **Contrasts With:** [[parallel-quicksort]].
