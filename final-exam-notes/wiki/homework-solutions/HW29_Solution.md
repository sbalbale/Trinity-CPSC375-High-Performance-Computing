# Homework 29 Solution
**CPSC 375: High-Performance Computing — Spring 2026**

---

## Problem 1

**Problem Statement:** In High-Performance Computing (HPC), the Transpose operation is often used between row and column operations. Explain why it is often faster or easier for a processor to communicate with neighbors in the same row rather than across columns. How does performing a transposition improve spatial locality during the sorting process? If you have a system where each process is assigned to exactly one row, describe the communication pattern required to perform a column-wise sort without using a transpose operation.

### Solution

**Row vs. Column Communication**

In a typical parallel matrix layout stored in row-major order, elements within the same row are contiguous in memory. When a processor needs to communicate with its row-neighbors, it accesses adjacent memory addresses — this is cache-friendly and results in few cache misses. Communicating across columns, on the other hand, requires jumping over `n` elements between each access (a stride of `n`), which is not contiguous in memory, causes frequent cache misses, and also typically means crossing process or node boundaries that add network latency.

Furthermore, each processor is typically assigned to exactly one row. This means row-neighbors share the same memory space or are directly adjacent in the process grid, making point-to-point communication cheap. Reaching a neighbor in the same column requires a message across a different row's process — usually a longer, slower communication path.

**How Transposition Improves Spatial Locality**

When performing column-wise operations (e.g., column sorting in Shearsort/column-sort), elements in the same column are spread far apart in row-major memory layout. By transposing the matrix first, what were columns become rows. Now those elements are contiguous in memory, and each processor can operate on them locally with good spatial locality. After the sort, a second transpose restores the original layout. This two-transpose trick converts a stride-n access pattern into a stride-1 (sequential) access pattern.

**Column-wise Sort Without Transposition**

Without using a transpose, if each process owns exactly one row of the matrix, performing a column-wise sort requires processes to collaborate across rows. The required communication pattern is:

1. **All-to-all or all-gather by column:** Every process must collect all elements from the same column — i.e., element `[0][j], [1][j], ..., [p-1][j]` — by exchanging data with every other process that holds part of that column.
2. **One process performs the sort:** A designated process (e.g., the one owning column `j` by some assignment) gathers, sorts, and redistributes the values back to all rows.
3. **Scatter results:** After sorting, the sorted column values are scattered back to each row-owning process.

This pattern requires O(p) point-to-point messages per column sort (where p is the number of rows/processes), resulting in O(p × n) total messages for n columns — far more expensive than the two-transpose approach, which only requires two all-to-all redistribution operations.

---

## Problem 2

**Problem Statement:** You are given the following unsorted input array: `[3, 0, 3, 2, 1, 1]`.

### Part A — Create the Initial Count Array

**Problem Statement:** Create the initial count array where the index represents the value of the elements.

The input values range from 0 to 3, so the count array has indices 0–3.

Count how many times each value appears:

| Value | Count |
|-------|-------|
| 0     | 1     |
| 1     | 2     |
| 2     | 1     |
| 3     | 2     |

**Initial count array:** `count = [1, 2, 1, 2]`

(Index `i` holds the number of occurrences of value `i`.)

---

### Part B — Prefix Sum Transformation

**Problem Statement:** Perform the prefix sum transformation on your count array. What does the value at each index in this transformed array represent?

Apply a running total (prefix sum) to the count array:

| Index | Before (count) | Prefix Sum Calculation   | After |
|-------|----------------|--------------------------|-------|
| 0     | 1              | 1                        | 1     |
| 1     | 2              | 1 + 2                    | 3     |
| 2     | 1              | 1 + 2 + 1                | 4     |
| 3     | 2              | 1 + 2 + 1 + 2            | 6     |

**Prefix-sum count array:** `count = [1, 3, 4, 6]`

**Interpretation:** After the prefix sum transformation, `count[i]` represents the number of elements in the input array that are **less than or equal to** value `i`. Equivalently, it gives the **last (rightmost) index + 1** in the output array where a value equal to `i` should be placed. So `count[i] - 1` is the final sorted position of the last occurrence of value `i`.

---

### Part C — Stable Placement (Right-to-Left Iteration)

**Problem Statement:** To maintain stability, the algorithm iterates through the original input array from right to left. Demonstrate the first two placements (the elements `1` and `1` at the end of the input) and show how the count array is updated after each placement.

**Input array (right-to-left):** `[3, 0, 3, 2, 1, 1]`
**Starting prefix-sum count array:** `count = [1, 3, 4, 6]`
**Output array** (size 6, 0-indexed): `output = [_, _, _, _, _, _]`

---

**Placement 1: Element at index 5 = `1`**

- Look up `count[1] = 3`.
- Place `1` at output position `3 - 1 = 2` (0-indexed).
- Decrement `count[1]` → `count[1] = 2`.

```
output = [_, _, 1, _, _, _]
count  = [1, 2, 4, 6]
```

---

**Placement 2: Element at index 4 = `1`**

- Look up `count[1] = 2`.
- Place `1` at output position `2 - 1 = 1` (0-indexed).
- Decrement `count[1]` → `count[1] = 1`.

```
output = [_, 1, 1, _, _, _]
count  = [1, 1, 4, 6]
```

**Note on stability:** Because we iterate right-to-left and decrement the count pointer after each placement, the two `1`s are placed at positions 1 and 2 respectively. The rightmost `1` (index 5) goes to position 2, and the next `1` (index 4) goes to position 1 — preserving their relative original order, which is what makes counting sort **stable**.
