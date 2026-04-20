---
aliases: [Contiguous Partitioning, BLOCK_LOW]
tags: [parallel-programming, mapping, data-partitioning]
sources: [lec23.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Block Decomposition

> [!abstract]
> **Block Decomposition** is a data partitioning strategy that divides a large array or domain into $p$ contiguous, roughly equal-sized chunks. It is used to **balance computational load** when the work associated with each element is spatially correlated.

## Core Mechanics

> [!equation] Mapping Macros (Method #2)
> Given $n$ elements and $p$ processes with ID $id \in [0, p-1]$:
> - **Low Index**: $L(id) = \lfloor (id \cdot n) / p \rfloor$
> - **High Index**: $H(id) = L(id+1) - 1$
> - **Block Size**: $S(id) = L(id+1) - L(id)$
> - **Owner of Index $j$**: $O(j) = \lfloor (p(j+1)-1) / n \rfloor$

> [!warning] Common Pitfalls
> - **Remainder Handling**: If $n$ is not a multiple of $p$, some blocks must be larger than others. The macros above handle this automatically by distributing remainders across the processes.
> - **Empty Blocks**: If $p > n$, some processes will be assigned zero elements. This is usually undesirable as it leads to idle resources.
> - **Load Imbalance**: While block decomposition is balanced for many problems, it can fail if work is concentrated in specific regions of the data (e.g., lower indices in Sieve).

## Implementations & Examples

> [!example] 13 elements, 5 processes
> Using Method #2:
> - $L(0) = \lfloor 0/5 \rfloor = 0$
> - $L(1) = \lfloor 13/5 \rfloor = 2$
> - $L(2) = \lfloor 26/5 \rfloor = 5$
> - $L(3) = \lfloor 39/5 \rfloor = 7$
> - $L(4) = \lfloor 52/5 \rfloor = 10$
> - $L(5) = \lfloor 65/5 \rfloor = 13$
> Blocks: [0,1], [2,3,4], [5,6], [7,8,9], [10,11,12]. Sizes: 2, 3, 2, 3, 3.

## Connections
* **Prerequisites:** [[data-partitioning]], [[mapping-parallel-programming]].
* **Used In:** [[sieve-of-eratosthenes]], [[trapezoidal-rule-using-mpi]].
* **Contrasts With:** [[interleaved-decomposition]] (Cyclic).
