---
aliases: [Non-Comparison Sort, Frequency Sort]
tags: [algorithm, sorting, stability]
sources: [lec29.txt, Homework 29.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Counting Sort

> [!abstract]
> **Counting Sort** is a non-comparison-based sorting algorithm that operates by counting the frequency of each distinct key value. It is **stable** and efficient for sorting integers within a relatively small range.

## Core Mechanics

### Detailed Algorithm
> [!code] Implementation Logic
> ```c
> // 1. Count frequencies
> for (i = 0; i < n; i++) count[A[i]]++;
> 
> // 2. Cumulative sum (prefix sum)
> for (i = 1; i < b; i++) count[i] += count[i-1];
> 
> // 3. Place elements (iterate RIGHT to LEFT for stability)
> for (i = n - 1; i >= 0; i--) {
>     int val = A[i];
>     int pos = count[val] - 1;
>     Output[pos] = val;
>     count[val]--;
> }
> ```

### Complexity Breakdown
- **Counting**: $O(n)$
- **Prefix Sum**: $O(b)$ where $b$ is the radix/base.
- **Placement**: $O(n)$
- **Total Time**: **$O(n + b)$**
- **Space**: $O(n + b)$ for the output and count arrays.

> [!warning] Stability
> Counting sort **must be stable** to be used as a primitive for [[radix-sort]]. This is achieved by iterating through the original list from **right to left** during the placement phase. This ensures that if two elements have the same value, the one that appeared later in the original array appears later in the sorted array.

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
