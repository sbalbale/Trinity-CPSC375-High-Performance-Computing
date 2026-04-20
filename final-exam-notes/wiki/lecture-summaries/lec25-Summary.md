---
aliases: [Lecture 25 Summary, Parallel Sorting I Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 25: Parallel Sorting Algorithms I

> [!abstract]
> This lecture introduces the fundamental primitives of parallel sorting, specifically the **Compare-and-Exchange** operation. It explores how simple sequential algorithms like Bubble Sort are adapted for parallelism and introduces high-level strategies for Mergesort and Quicksort.

## Sorting Primitives

### Compare-and-Exchange
The basic operation where two values $A$ and $B$ are compared and their positions swapped if they are out of order.
- **MPI Implementation**: One process sends its value to another; the receiver compares both and sends back the appropriate value.
- **Optimization**: Both processes send their values to each other simultaneously. Both perform the comparison locally. This reduces the number of sequential communication steps.

### Merge-and-Split (for $n > p$)
When each process holds multiple elements (a local sorted list):
1. **Exchange**: Adjacent processes exchange their entire sorted lists.
2. **Merge**: Each process merges its local list with the received list ($O(n/p)$).
3. **Split**: The process with the lower rank keeps the **lower half** of the merged list; the process with the higher rank keeps the **higher half**.

## Parallel Sorting Algorithms

### 1. Odd-Even Transposition Sort
A parallel variation of Bubble Sort that operates in two alternating phases:
- **Even Phase**: Processes $(0,1), (2,3), \dots$ exchange and compare.
- **Odd Phase**: Processes $(1,2), (3,4), \dots$ exchange and compare.
After $p$ phases, the list is guaranteed to be sorted.

### 2. Parallel Mergesort
- **Divide**: A tree of processes is used. The master process divides the list and sends halves down the tree.
- **Sort**: At the leaves, the list is sorted sequentially.
- **Merge**: Sorted sublists are sent back up the tree and merged at each internal node.

### 3. Parallel Quicksort
- **Pivot Selection**: A pivot is chosen (often by the master).
- **Partitioning**: Elements are compared against the pivot and divided into two groups.
- **Recursion**: Groups are assigned to different sub-trees of processes.

> [!warning] Floating-Point SPMD Hazards
> If comparisons are duplicated on different processors (e.g., to reduce communication), differences in **floating-point precision** or hardware registers could lead to different boolean results on different ranks. This can cause the processes to diverge, breaking the SPMD (Single Program, Multiple Data) execution model.
