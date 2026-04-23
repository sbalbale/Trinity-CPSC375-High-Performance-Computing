---
aliases: [Parallel Sorts, Complexity Sheet]
tags: [#exam/algorithm, #prep/moc, #course_hpc]
sources: [lec25.md, lec28.md, lec29.md, Homework-25-Solutions.md, Homework-28-Solutions.md]
created: 2026-04-20
updated: 2026-04-20
---

# Cheat Sheet: Sorting Algorithms

> [!abstract]
> Parallel mechanics and time complexity for common sorting algorithms.

## 1. Comparison Sorts
| Algorithm | Best Case | Average Case | Worst Case | Parallel Time |
| :--- | :--- | :--- | :--- | :--- |
| **Odd-Even** | $O(n \log n)$ | $O(n \log n)$ | $O(n \log n)$ | $O(\log^2 n)$ |
| **Mergesort** | $O(n \log n)$ | $O(n \log n)$ | $O(n \log n)$ | $O(\log^2 n)$ |
| **Quicksort** | $O(n \log n)$ | $O(n \log n)$ | $O(n^2)$ | Variable |
| **Bitonic** | $O(n \log^2 n)$| $O(n \log^2 n)$| $O(n \log^2 n)$| $O(\log^2 n)$ |

## 2. Special Network Sorts
| Algorithm | Topology | Time | Notes |
| :--- | :--- | :--- | :--- |
| **Shearsort** | 2D Mesh | $O(\sqrt{n} \log \sqrt{n})$ | Snake order; $\log N$ iterations. |
| **Bitonic** | Hypercube | $O(\log^2 n)$ | Regular dimension-based C&E. |

## 3. Non-Comparison Sorts
| Algorithm | Time | Notes |
| :--- | :--- | :--- |
| **Counting Sort**| $O(n+b)$ | **Stable** base for Radix sort. |
| **Radix Sort** | $O(d(n+b))$ | Uses `MPI_Exscan` for global offsets. |

## 4. Sorting Primitives
> [!code]
> - **Compare-and-Exchange:** Basic $A \leftrightarrow B$ swap.
> - **Merge-and-Split:** For $n > p$. Each process keeps half of merged list.

## 5. Bitonic Sequence
> [!info]
> - **Definition:** Sequence that increases then decreases (or cyclic shift).
> - **Split Property:** Comparing $a_i \leftrightarrow a_{i+n/2}$ results in two smaller bitonic sequences where one is globally smaller than the other.
