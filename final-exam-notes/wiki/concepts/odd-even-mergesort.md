---
aliases: [Recursive Odd-Even Merge, Batcher's Merge]
tags: [algorithm, sorting, parallel-programming]
sources: [lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Odd-Even Mergesort

> [!abstract]
> **Odd-Even Mergesort** is a parallel sorting algorithm that merges two sorted lists by recursively merging their odd and even indexed elements separately. It is a **Compare-and-Exchange** network that sorts $n$ elements in $O(\log^2 n)$ parallel time.

## Core Mechanics

### The Merge Step
To merge two sorted lists $A = \{a_1, \dots, a_n\}$ and $B = \{b_1, \dots, b_n\}$:
1. **Odd Merge**: Form list $C$ by merging $\{a_1, a_3, \dots, a_{n-1}\}$ and $\{b_1, b_3, \dots, b_{n-1}\}$.
2. **Even Merge**: Form list $D$ by merging $\{a_2, a_4, \dots, a_n\}$ and $\{b_2, b_4, \dots, b_n\}$.
3. **Interleave**: The final sorted list $E$ is formed by interleaving $C$ and $D$.
4. **Compare-and-Exchange**: Perform a final pass of comparisons on adjacent elements: $e_{2i} = \min(c_{i+1}, d_i)$ and $e_{2i+1} = \max(c_{i+1}, d_i)$.

> [!warning] Power of Two
> The standard Batcher algorithm assumes $n$ is a power of two ($n = 2^k$). If $n$ is not a power of two, the list must be padded with "dummy" values (infinity).

## Implementations & Examples

> [!example] Merge Example (4 elements)
> $A=\{2, 4\}$, $B=\{1, 3\}$
> - Odd indices: $\{2\}, \{1\} \to \text{Merge} \to \{1, 2\}$ ($C$)
> - Even indices: $\{4\}, \{3\} \to \text{Merge} \to \{3, 4\}$ ($D$)
> - Interleave: $\{1, 3, 2, 4\}$
> - Final Compare (middle pair 3, 2): $\{1, \min(3,2), \max(3,2), 4\} \to \{1, 2, 3, 4\}$.

## Connections
* **Prerequisites:** [[compare-and-exchange]].
* **Used In:** [[batchers-sorting-networks]].
* **Contrasts With:** [[bitonic-mergesort]].
