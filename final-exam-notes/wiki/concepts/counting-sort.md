---
aliases: [Non-Comparison Sort, Frequency Sort]
tags: [algorithm, sorting, stability]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Counting Sort

> [!abstract]
> **Counting Sort** is a non-comparison-based sorting algorithm that operates by counting the frequency of each distinct key value. It is **stable** and efficient for sorting integers within a relatively small range.

## Core Mechanics

### The Algorithm
1. **Count**: Create an auxiliary array `count` of size $b$ (the range of inputs). For each element $x$ in the input, increment `count[x]`.
2. **Cumulative Sum**: Transform `count` by adding each element to its predecessor ($O(b)$). Each entry now contains the final position for its value.
3. **Place**: Iterate through the original array (from right to left to ensure stability) and use `count[x]` to place $x$ into its final position in an output array.

> [!equation] Time Complexity
> - Total Time: $O(n + b)$ where $n$ is elements and $b$ is the range.
> - Space Complexity: $O(n + b)$.

> [!warning] Stability
> Counting sort **must be stable** to be used as a primitive for [[radix-sort]]. This is achieved by iterating through the original list from right to left in the final placement step.

## Implementations & Examples

> [!example] Counting Sort Flow
> Input: `[1, 4, 1, 2, 5, 2]`
> 1. Frequencies: `count = [0, 2, 2, 0, 1, 1]`
> 2. Cumulative Sums: `csum = [0, 2, 4, 4, 5, 6]`
> 3. Placement: First `2` from right is placed at index 4, second `2` at index 3. Result: `[1, 1, 2, 2, 4, 5]`.

## Connections
* **Prerequisites:** Basic array operations.
* **Used In:** [[radix-sort]].
* **Contrasts With:** Comparison-based sorting algorithms like [[parallel-mergesort]].
