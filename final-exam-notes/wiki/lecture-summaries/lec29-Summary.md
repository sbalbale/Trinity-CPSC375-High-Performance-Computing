---
aliases: [Lecture 29 Summary, Parallel Sorting III Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 29: Parallel Sorting Algorithms III

> [!abstract]
> This lecture discusses specialized parallel sorting algorithms for Mesh networks, such as **Shearsort**, and transitions into non-comparison sorting techniques like **Counting Sort** and **Radix Sort**. It specifically addresses the challenges of implementing a distributed parallel Radix Sort using MPI.

## Mesh-Based Sorting

### Shearsort
A 2D sorting algorithm designed for an $N \times N$ mesh of $n$ elements.
- **Phases**:
    1. **Row Sort**: Sort even rows increasing, odd rows decreasing (snakelike order).
    2. **Column Sort**: Sort columns top-to-bottom.
- **Convergence**: Completes in $\log N$ iterations.
- **Time Complexity**: $O(\sqrt{n} \log \sqrt{n})$ in parallel.

### Mesh Optimization via Transposition
Restricting communication to one dimension is often faster and improves cache locality.
- **Method**: Insert a **Transposition** step between row and column operations to ensure all data movements happen within rows.

---

## Non-Comparison Sorting

### Counting Sort
A non-comparison sort that works in $O(n+b)$ time where $b$ is the range of values.
1. **Count**: Create a frequency array for each distinct value.
2. **Cumulative Sum (Prefix Sum)**: Transform counts into final array positions.
3. **Place**: Iterate (right-to-left for stability) and place elements into the output array.

### Radix Sort (LSD)
Uses Counting Sort as a stable primitive to sort numbers by their positional digits, starting from the Least Significant Digit (LSD).
- **Time Complexity**: $O(d(n+b))$ for $d$ digits.
- **Serial Example**: Unsorted `[170, 45, 75, 90, 802, 24, 2, 66]` $\to$ Sorted by 1s $\to$ Sorted by 10s $\to$ Sorted by 100s.

---

## Distributed Parallel Radix Sort (Assignment 8)
Parallelizing Radix Sort requires global coordination to maintain stability.
- **Collective Communication**:
    - `MPI_Exscan`: Used to calculate the global prefix sums (offsets) across all processes for each bucket.
    - `MPI_Alltoallv`: Used to distribute the elements to their new owner nodes according to their bucket values.
- **Radix Selection**: Often uses $b=256$ (one byte at a time) for 32-bit integers.
