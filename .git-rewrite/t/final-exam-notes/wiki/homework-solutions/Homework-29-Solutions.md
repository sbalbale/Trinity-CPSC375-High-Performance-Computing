---
tags: [#homework/solutions, #exam/algorithm, #exam/theory]
sources: [Homework 29.pdf]
created: 2026-04-21
updated: 2026-04-21
aliases: [HW29]
---

# Homework 29 Solutions

>[!abstract]
> Solutions to Homework 29 covering Transpose operations for spatial locality and the mechanics of Counting Sort.

## 1. Transpose Operations and Spatial Locality

>[!example] Transpose vs Column-wise Sort
> **Question:** How does performing a transposition improve spatial locality during the sorting process? If you have a system where each process is assigned to exactly one row, describe the communication pattern required to perform a column-wise sort without using a transpose operation.
> 
> **Solution:**
> - **Spatial Locality:** In C/C++, matrices are stored in row-major order. A transpose operation reorganizes column data into rows. Traversing data row-by-row means memory addresses are accessed contiguously, which maximizes cache line utilization and minimizes cache misses.
> - **Communication Pattern:** If processes are mapped to rows, sorting a column requires elements from every row. Without a transpose, performing a column-wise sort requires an **all-to-all communication pattern**. Every process must communicate with every other process to exchange elements for each column, resulting in high communication overhead.

## 2. Counting Sort Execution

>[!example] Counting Sort on [3, 0, 3, 2, 1, 1]
> **Question:** Trace the execution of Counting Sort on the input array `[3, 0, 3, 2, 1, 1]`.
> 
> **A. Initial Count Array:**
> The input values range from 0 to 3, so we create a count array of size 4.
> - `[1, 2, 1, 2]` (There is one 0, two 1s, one 2, and two 3s)
> 
> **B. Prefix Sum Transformation:**
> We perform an inclusive prefix sum on the count array:
> - `[1, 3, 4, 6]`
> **Meaning:** The value at each index represents the 1-based ending position (or the number of elements less than or equal to that index value) in the final sorted array.
> 
> **C. Placement and Stability:**
> To maintain stability, we iterate through the original array from right to left: `[3, 0, 3, 2, 1, 1]`
> 
> 1. **First element (1):** 
>    - The count for `1` is `3`. 
>    - Place `1` at 0-based index `3 - 1 = 2`.
>    - Output array: `[_, _, 1, _, _, _]`
>    - Decrement count for `1`. Count array becomes: `[1, 2, 4, 6]`
> 2. **Second element (1):**
>    - The count for `1` is now `2`.
>    - Place `1` at 0-based index `2 - 1 = 1`.
>    - Output array: `[_, 1, 1, _, _, _]`
>    - Decrement count for `1`. Count array becomes: `[1, 1, 4, 6]`