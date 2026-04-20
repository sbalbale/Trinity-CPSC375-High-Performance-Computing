---
aliases: [Bitonic Sort, Recursive Bitonic Mergesort]
tags: [algorithm, sorting, parallel-programming]
sources: [lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Bitonic Mergesort

> [!abstract]
> **Bitonic Mergesort** is an $O(\log^2 n)$ parallel sorting algorithm that recursively builds larger and larger **bitonic sequences** until a single bitonic sequence representing the entire list is sorted into an increasing or decreasing list.

## Core Mechanics

### General Algorithm
To sort an arbitrary sequence:
1. **Phases**: The sort is performed in $k = \log_2 n$ phases.
2. **Phase $j$**: In phase $j$, the list is divided into $n/2^j$ sub-blocks of size $2^j$. Each sub-block is converted into a **bitonic sequence** (half increasing, half decreasing).
3. **Sorting Bitonic Sequences**: Each bitonic sequence is sorted according to the **Bitonic Split Property**.

> [!equation] Parallel Steps
> For $n = 2^k$ elements, the number of parallel comparison steps is:
> $$Steps = \sum_{i=1}^{k} i = \frac{k(k+1)}{2} = O(\log^2 n)$$

> [!warning] Hardware Mapping
> Because bitonic sort uses regular, power-of-two communication patterns (e.g., $i \leftrightarrow i+n/2$), it maps exceptionally well to **hypercube** network topologies.

## Implementations & Examples

> [!example] Phase 1 for 4 elements $\{10, 2, 5, 8\}$
> 1. Sort $(10, 2) \to (2, 10)$ (increasing)
> 2. Sort $(5, 8) \to (8, 5)$ (decreasing)
> 3. Result: $\{2, 10, 8, 5\}$ (A **bitonic sequence**)
> 4. Phase 2 (Split): Compare $2$ with $8$, $10$ with $5 \to \{2, 5, 8, 10\}$ (Sorted)

## Connections
* **Prerequisites:** [[bitonic-sequence]], [[compare-and-exchange]].
* **Used In:** [[batchers-sorting-networks]].
* **Contrasts With:** [[odd-even-mergesort]].
