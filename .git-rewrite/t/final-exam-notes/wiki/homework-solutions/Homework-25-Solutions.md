---
aliases: [Homework 25 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi, #sorting]
sources: [Homework 25.pdf, lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 25 Solutions

> [!abstract]
> Solutions to the parallel sorting exercises for Homework 25, covering the mechanics, complexity, and implementation of Odd-Even Transposition Sort and Parallel Mergesort.

## Problem 1: Odd-Even Transposition Sort

### A. Mechanics and Phases
- **Why suitable for parallel execution:** Within a single phase (odd or even), the comparison-exchange pairs are **mutually exclusive**. For example, in an even phase, pairs $(0,1), (2,3), (4,5)$ are all independent and can be processed simultaneously by different processors.
- **Phases needed:** For $n$ elements, **$n$ phases** are needed in the worst case to guarantee the list is sorted.
- **Skipping a phase:** If an odd or even phase is skipped, "stranded" elements may remain out of order. The algorithm relies on the alternating phases to move values across both even and odd indices to their final sorted positions.

### B. MPI Implementation Pattern
> [!code] MPI Logic
> ```c
> for (int phase = 0; phase < p; phase++) {
>     if (phase % 2 == 0) { // Even Phase
>         if (my_rank % 2 == 0) {
>             // P0 interacts with P1, P2 with P3
>             if (my_rank < p - 1) CompareAndExchange(my_rank + 1);
>         } else {
>             CompareAndExchange(my_rank - 1);
>         }
>     } else { // Odd Phase
>         if (my_rank % 2 != 0) {
>             // P1 interacts with P2, P3 with P4
>             if (my_rank < p - 1) CompareAndExchange(my_rank + 1);
>         } else if (my_rank > 0) {
>             CompareAndExchange(my_rank - 1);
>         }
>     }
> }
> ```

---

## Problem 2: Parallel Mergesort

### A. Complexity and Efficiency
**Assuming Unlimited Processors:**
- **Parallel Time Complexity:** **$O(n)$**. While the splitting phase is $O(\log n)$, the final merge operation at the top of the tree must process all $n$ elements. Even with unlimited processors, a standard sequential merge takes linear time.
- **Total Work Performed:** **$O(n \log n)$**. This is the same as the sequential complexity.
- **Work-Efficient:** **Yes**. An algorithm is work-efficient if its parallel work is asymptotically equal to the best sequential work. Since parallel mergesort performs $O(n \log n)$ work, it matches the optimal sequential bound.

### B. MPI implementation Pattern
1. Each process sorts its local $n/p$ list using a sequential sort.
2. **Merge Tree Phase:**
    - Processes with odd ranks send their sorted list to their even-ranked neighbor.
    - Even-ranked processes receive the list and merge it with their own.
    - In the next level of the tree, every 4th rank receives from its neighbor, and so on.
    - This continues until rank 0 has the fully merged list.

---

## Problem 3: Synchronization Impact

**Why is synchronization required?**
Parallel sorting algorithms require synchronization between phases to ensure that all data movements and comparisons from level $i$ are complete before any process starts operations for level $i+1$. Without synchronization, a process might attempt to merge data that has not yet been received or sorted by its partner.

**Performance Impact:**
Synchronization introduces **overhead** and **serialization**. Faster processors must wait for the slowest processor to reach the synchronization point ([[mpi-barrier]] or implicit message blocking), reducing overall [[parallel-efficiency]].

**Which algorithm is most affected?**
**Odd-Even Transposition Sort** is most affected by synchronization overhead. It requires $n$ (or $p$) synchronization phases, each involving global communication. In contrast, Mergesort only requires $\log p$ synchronization levels in its merge tree.
