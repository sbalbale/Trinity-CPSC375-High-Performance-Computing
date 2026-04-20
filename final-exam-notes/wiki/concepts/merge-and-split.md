---
aliases: [Merge-and-Keep, Scaling Sorting]
tags: [algorithm, sorting, mpi]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Merge-and-Split

> [!abstract]
> **Merge-and-Split** is the parallel scaling of the [[compare-and-exchange]] primitive for cases where $n > p$. Each process holds a **sorted local list** of size $n/p$. They exchange these lists, merge them, and then each process keeps either the lower or upper half of the results.

## Core Mechanics

1. **Local Sort**: Each process independently sorts its $n/p$ elements.
2. **Exchange**: Adjacent processes exchange their sorted lists ($O(n/p)$).
3. **Merge**: Each process merges its own list with the received list to form a temporary list of size $2n/p$ ($O(n/p)$).
4. **Split**: 
    - The process with the **lower rank** keeps the **lower $n/p$ elements**.
    - The process with the **higher rank** keeps the **higher $n/p$ elements**.

> [!warning] Load Balancing
> If the list size $n$ is not a multiple of the process count $p$, the "halves" kept after the split will have slightly different sizes, and mapping must account for this (using [[block-decomposition]]).

## Implementations & Examples

> [!example] Merge-Split Workflow
> - P1 (Lower) has `{25, 43, 50, 88}`.
> - P2 (Higher) has `{28, 42, 80, 98}`.
> 1. Exchange: Both now have both lists.
> 2. Merge: Both form `{25, 28, 42, 43, 50, 80, 88, 98}`.
> 3. Split: P1 keeps `{25, 28, 42, 43}`, P2 keeps `{50, 80, 88, 98}`.

## Connections
* **Prerequisites:** [[compare-and-exchange]].
* **Used In:** Scalable implementations of parallel sorting algorithms.
* **Contrasts With:** Simple compare-and-exchange of single values.
