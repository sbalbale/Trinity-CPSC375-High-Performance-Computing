---
aliases: [Row-Column Mesh Sort]
tags: [algorithm, sorting, mesh]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Shearsort

> [!abstract]
> **Shearsort** is a 2D parallel sorting algorithm designed for an $N \times N$ mesh of processors. It sorts a sequence by alternating between row and column sorting, resulting in a **snakelike** sorted order.

## Core Mechanics

### The Algorithm
For a mesh of $p$ processors (arranged as $\sqrt{p} \times \sqrt{p}$):
1. **Row Phase (Even Rows)**: Sort from left to right.
2. **Row Phase (Odd Rows)**: Sort from right to left (**Snake Order**).
3. **Column Phase**: Sort all columns from top to bottom.
4. **Repeat**: Perform steps 1-3 for **$\lceil \log_2 p \rceil + 1$** iterations.

> [!equation] Time Complexity
> - Convergence: $O(\log \sqrt{p})$ iterations.
> - Parallel Time: $O(\sqrt{n} \log \sqrt{n})$ where $n$ is the number of elements.

> [!warning] Hardware Advantage
> Shearsort is simple to implement because it only requires 1D sorting primitives, making it a common choice for mesh-connected parallel hardware and systolic arrays.

## Implementations & Examples

> [!example] 4x4 Mesh Flow
> Iteration 1 sorts all 4 rows (0,2 R, 1,3 L) then all 4 columns. This process continues for 3 iterations ($1 + \log_2 4$).

## Connections
* **Prerequisites:** [[mapping-parallel-programming]] (Mesh).
* **Used In:** Hardware-based mesh sorting.
* **Contrasts With:** [[bitonic-mergesort]] (Hypercube).
