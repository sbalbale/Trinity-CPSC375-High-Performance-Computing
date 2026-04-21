# Lecture 29: Parallel Sorting Algorithms III - Radix Sort

**Course:** CPSC 375: High-Performance Computing  
**Spring 2026** | Trinity College  
**Instructor:** Prof. Yoon

---

## Overview

This lecture introduces **Radix Sort**, a non-comparison-based sorting algorithm with excellent parallelization properties.

Topics covered:
- Radix Sort fundamentals
- Least Significant Digit (LSD) vs. Most Significant Digit (MSD) approaches
- Counting Sort subroutine
- Parallel implementation strategies
- Performance analysis and assignment

---

## Why Radix Sort?

### Limitations of Comparison-Based Sorts

Comparison-based algorithms (quicksort, mergesort, bitonic) have a **lower bound** of $\Omega(n \log n)$.

### Radix Sort Advantage

Radix Sort is **non-comparison-based** and achieves $O(d(n + b))$ time, where:
- $d$ = number of digit positions (passes)
- $n$ = number of elements
- $b$ = base/radix of the number system

For fixed $d$ and $b$, this is **linear in $n$**!

---

## Counting Sort: The Foundation

Radix Sort relies on **Counting Sort** as a subroutine.

### Concept

Counting Sort works by:
1. Counting occurrences of each key value
2. Using counts to determine final positions
3. Placing elements in sorted order

### Algorithm: Counting Sort

**Input:** Array of values in range [0, b)

**Step 1: Count frequencies**

```
count[0 to b-1] = 0
for each element a[i]:
    count[a[i]]++
```

**Step 2: Cumulative sum (prefix sum)**

```
csum[0] = count[0]
for i = 1 to b-1:
    csum[i] = csum[i-1] + count[i]
```

This tells us: elements with value $i$ go to positions `csum[i-1]` through `csum[i]-1`.

**Step 3: Place elements**

```
// Iterate RIGHT to LEFT to maintain stability
for i = n-1 down to 0:
    value = a[i]
    position = csum[value] - 1
    output[position] = a[i]
    csum[value]--
```

### Example: Counting Sort

**Input:** `[1, 4, 1, 2, 5, 2]`

**Step 1: Frequencies**
```
count = [0, 2, 2, 0, 1, 1]
         ↑  ↑  ↑  ↑  ↑  ↑
         0  1  2  3  4  5
```

**Step 2: Cumulative sum**
```
csum = [0, 2, 4, 4, 5, 6]
```

Interpretation:
- Value 0: positions [0, 0) — nowhere
- Value 1: positions [0, 2) — positions 0-1
- Value 2: positions [2, 4) — positions 2-3
- Value 3: positions [4, 4) — nowhere
- Value 4: positions [4, 5) — position 4
- Value 5: positions [5, 6) — position 5

**Step 3: Place elements (right to left)**

```
i=5: a[5]=2, position = csum[2]-1 = 3,  output[3] = 2, csum[2]--
i=4: a[4]=5, position = csum[5]-1 = 5,  output[5] = 5, csum[5]--
i=3: a[3]=2, position = csum[2]-1 = 2,  output[2] = 2, csum[2]--
i=2: a[2]=1, position = csum[1]-1 = 1,  output[1] = 1, csum[1]--
i=1: a[1]=4, position = csum[4]-1 = 4,  output[4] = 4, csum[4]--
i=0: a[0]=1, position = csum[1]-1 = 0,  output[0] = 1, csum[1]--

Result: [1, 1, 2, 2, 4, 5]  ✓ Sorted!
```

### Counting Sort Complexity

- **Time:** $O(n + b)$
  - Counting: $O(n)$
  - Cumulative sum: $O(b)$
  - Placement: $O(n)$

- **Space:** $O(n + b)$ for output and auxiliary arrays

- **Stability:** Yes (placing right-to-left preserves original order for equal elements)

---

## Radix Sort: Least Significant Digit (LSD)

### Concept

Sort by processing digits from **least to most significant**.

After each pass, elements are partially sorted. After all passes, fully sorted.

### Algorithm

```
for digit_pos = 0 to num_digits-1:
    sort array using counting sort on digit at digit_pos
    (result becomes input for next iteration)
```

### Why It Works

**Key property:** Counting Sort is **stable**.

After sorting by digit $i$, elements are ordered by digits 0 through $i$, because:
- Stable sort preserves relative order of equal elements
- Elements equal in digit $i$ retain order from previous digit

### Example: LSD Radix Sort (Base 10)

**Input:** `[170, 45, 75, 90, 802, 24, 2, 66]`

**Pass 1: Sort by least significant digit (ones place)**
```
Input:  [170, 45, 75, 90, 802, 24, 2, 66]
Sorted: [170, 90, 802, 2, 24, 45, 75, 66]
        (by last digit: 0, 5, 2, 0, 4, 5, 5, 6)
```

**Pass 2: Sort by tens digit**
```
Input:  [170, 90, 802, 2, 24, 45, 75, 66]
Sorted: [802, 2, 24, 45, 66, 170, 75, 90]
        (by tens digit: 0, 0, 2, 4, 6, 7, 7, 9)
```

**Pass 3: Sort by hundreds digit**
```
Input:  [802, 2, 24, 45, 66, 170, 75, 90]
Sorted: [2, 24, 45, 66, 75, 90, 170, 802]
        ✓ FULLY SORTED!
```

### LSD Complexity

- **Time:** $O(d(n + b))$
  - $d$ passes through the data
  - Each pass: $O(n + b)$ counting sort
  
- **Space:** $O(n + b)$

For fixed base $b$ and fixed digit length $d$: **$O(n)$**

---

## Radix Sort: Most Significant Digit (MSD)

### Concept

Sort by processing digits from **most to least significant**.

Recursively partition the array based on each digit.

### Key Difference from LSD

MSD is **recursive** and can:
- Skip sorting when a digit partition is small
- Benefit from cache locality
- Be harder to parallelize uniformly

### MSD Example

```
[170, 45, 75, 90, 802, 24, 2, 66]

First digit (hundreds):
  0xx: [45, 75, 90, 24, 2, 66]     → recurse
  1xx: [170]                        → sorted
  8xx: [802]                        → sorted

For [45, 75, 90, 24, 2, 66] (sort by tens):
  0x: [2, 66]     → sorted
  2x: [24]        → sorted
  4x: [45]        → sorted
  6x: [66]        → sorted
  7x: [75]        → sorted
  9x: [90]        → sorted
```

### MSD Complexity

- **Best case:** $O(n)$ — if partitions are small
- **Average case:** $O(d(n + b))$ — similar to LSD
- **Worst case:** $O(n \log n)$ — when partitioning fails

---

## Parallel Radix Sort

### Challenges

1. **Counting Sort communication:**
   - Computing global prefix sums
   - Distributing elements to correct positions

2. **Load balancing:**
   - Digit distributions may be skewed
   - Some processors idle during counting phase

3. **Synchronization:**
   - Barriers between passes
   - Global reduce operations

### Parallel Implementation Strategy

**Distributed memory approach (MPI):**

1. **Each process holds $n/p$ elements**

2. **For each digit:**
   - Each process: Local count
   - Reduce: Global prefix sums (MPI_Exscan, MPI_Allreduce)
   - Each process: Place elements locally
   - Redistribute: All-to-all exchange (MPI_Alltoallv)

3. **After all passes: Globally sorted data**

### MPI Functions for Radix Sort

```c
// Local counting and prefix sum
MPI_Exscan(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, comm);

// All-to-all personalized communication
MPI_Alltoallv(sendbuf, sendcounts, sdispls,
              recvbuf, recvcounts, rdispls, MPI_INT, comm);
```

### Communication Complexity

- **Per pass:** $O(\log p)$ for reduction, $O(n/p)$ for redistribution
- **Total:** $O(d \log p + d \cdot n/p)$

For good scalability: Computation $n/p$ must dominate communication.

---

## Practical Considerations

### Base Selection

Larger base → fewer passes but larger counting arrays:

| Base | Digits (32-bit) | Cost |
|------|-----------------|------|
| 2 | 32 | Many passes |
| 4 | 16 | Fewer passes |
| 16 | 8 | Balance |
| 256 | 4 | Good for 32-bit |

**Typical choice:** $b = 256$ (sorts 32-bit integers in 4 passes)

### Load Balancing

With skewed digit distribution:
- Some processors have few elements to sort
- Others have many
- **Solution:** Dynamic load balancing, uneven data distribution

### Comparison with Other Algorithms

| Algorithm | Time | Space | Parallel Time |
|-----------|------|-------|----------------|
| Bitonic | $O(n \log^2 n)$ | $O(n)$ | $O(\log^2 n)$ |
| Mergesort | $O(n \log n)$ | $O(n)$ | $O(\log n)$ |
| Radix (LSD) | $O(d \cdot n)$ | $O(n)$ | $O(d \cdot n/p)$ |

Radix sort is best when $d$ is small relative to $\log n$.

---

## Assignment: Parallel Radix Sort with MPI

### Requirements

Implement a **distributed-memory LSD Radix Sort** in C using MPI:

**Specifications:**
- Sort 32-bit unsigned integers
- Use **power-of-two radix** ($b = 256$)
- Ensure **stability** across nodes
- Implement **global coordination** using:
  - `MPI_Exscan` for prefix sums
  - `MPI_Alltoallv` for redistribution
- Develop on the **Pine cluster**

**Deliverables:**
1. Working C implementation
2. Performance benchmarks
3. Speedup analysis (use Gustafson's Law)
4. Report on scalability

### Testing Strategy

- Test correctness with small arrays
- Verify stability (equal elements preserve order)
- Benchmark with varying problem sizes
- Scale from 2 to 16 nodes
- Measure communication vs. computation time

---

## Summary

**Radix Sort** is a non-comparison-based algorithm with:
- Linear time complexity for bounded digit length
- Excellent parallelization properties
- Global communication through prefix sums and redistribution
- Superior practical performance on certain datasets

Key concepts:
- ✓ Counting Sort foundation
- ✓ Stability preservation through sorted passes
- ✓ Distributed implementation with MPI
- ✓ Load balancing challenges
- ✓ Practical base selection

---

## Next Lecture

Matrix multiplication algorithms and distributed linear algebra!
