---
aliases: [Odd-Even Sort, Parallel Bubble Sort]
tags: [algorithm, sorting, mpi]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Odd-Even Transposition Sort

> [!abstract]
> **Odd-Even Transposition Sort** is a highly regular parallel sorting algorithm that is a variation of **Bubble Sort**. It operates in $n$ alternating **even** and **odd** phases, where adjacent pairs of elements (or processes) compare and swap their values.

## Core Mechanics

> [!equation] Time Complexity
> - Sequential: $O(n^2)$
> - Parallel: $O(n)$ steps with $n$ processes ($n$ comparison steps).
> - Scaled: $O(p \cdot n/p)$ (using [[merge-and-split]]) with $p$ processes.

### Algorithm Phases
1. **Even Phase**: Compare and exchange pairs $(0, 1), (2, 3), (4, 5), \dots$.
2. **Odd Phase**: Compare and exchange pairs $(1, 2), (3, 4), (5, 6), \dots$.
3. Repeat the sequence $n/2$ times (total of $n$ phases).

> [!warning] Deadlock Risk
> In MPI, if both processes in a pair call `MPI_Recv` simultaneously, they will **deadlock**. A common solution is to have odd-numbered processes `send` then `recv`, while even-numbered processes `recv` then `send`.

## Implementations & Examples

> [!example] Odd-Even Transposition Flow
> Given list `{8, 5, 2, 9}`:
> 1. **Even Phase** (pairs 0-1, 2-3): `{5, 8, 2, 9}`
> 2. **Odd Phase** (pairs 1-2): `{5, 2, 8, 9}`
> 3. **Even Phase** (pairs 0-1, 2-3): `{2, 5, 8, 9}` (Sorted)

## Connections
* **Prerequisites:** [[compare-and-exchange]], [[merge-and-split]].
* **Used In:** Sorting on mesh-connected or linear array processors.
* **Contrasts With:** Sequential Bubble Sort.
