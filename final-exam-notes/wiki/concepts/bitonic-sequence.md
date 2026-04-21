---
aliases: [Bitonic Sequence Property]
tags: [algorithm, sorting, mathematics]
sources: [lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Bitonic Sequence

> [!abstract]
> A **Bitonic Sequence** is a sequence $\{a_0, \dots, a_{n-1}\}$ that monotonically increases and then monotonically decreases, or a sequence that can be **cyclically shifted** to have this property. They are the fundamental unit processed by Bitonic Mergesort.

## Core Mechanics

### Definition
A **bitonic sequence** is one with up to two monotonic pieces:
- A sequence increasing then decreasing ($\nearrow \searrow$), OR
- A sequence decreasing then increasing ($\searrow \nearrow$) - typically due to a cyclic shift, OR
- A purely monotonic sequence (increasing or decreasing).

> [!example] Examples
> - `[1, 3, 5, 7, 6, 4, 2]` (Inc then Dec)
> - `[6, 4, 2, 1, 3, 5, 7]` (Dec then Inc - cyclic shift)
> - `[1, 2, 3, 4, 5]` (Purely Inc)

### The Bitonic Split Property
If we perform a **compare-and-exchange** operation on $a_i$ with $a_{i+n/2}$ for all $i$:
1. The result is **two smaller bitonic sequences** of length $n/2$.
2. All numbers in one sequence are **less than** all numbers in the other sequence.

> [!warning] Cyclic Shifting
> A sequence like $\{5, 7, 9, 8, 6, 4, 3\}$ is bitonic. A cyclically shifted version like $\{9, 8, 6, 4, 3, 5, 7\}$ is also bitonic.

## Implementations & Examples

> [!example] Sorting a Bitonic Sequence
> A bitonic sequence of length 8 can be sorted into an increasing sequence by repeatedly applying the bitonic split until each sub-sequence has a length of 1.

## Connections
* **Prerequisites:** [[compare-and-exchange]].
* **Used In:** [[bitonic-mergesort]].
* **Contrasts With:** Monotonic sequence (only increases or only decreases).
