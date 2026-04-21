# Lecture 28: Parallel Sorting Algorithms II

**Course:** CPSC 375: High-Performance Computing  
**Spring 2026** | Trinity College  
**Instructor:** Prof. Yoon

---

## Overview

This lecture covers advanced parallel sorting algorithms, building on performance analysis principles:

- Review of Amdahl's and Gustafson's Laws in context
- Odd-Even Mergesort
- Bitonic Mergesort
- Parallel sorting strategies

---

## Performance Analysis Review

### Amdahl's Law vs. Gustafson's Law

**Amdahl's Law:** Fixed problem size ⇒ speedup limited by serial fraction (strong scaling)

**Gustafson's Law:** Scaled problem size ⇒ speedup grows with processors as parallel work increases (weak scaling)

### Karp-Flatt Metric

The **Karp-Flatt metric** provides an empirical measure to extract the effective serial fraction from observed speedup:

$$e = \frac{1/S_p - 1/p}{1 - 1/p}$$

Where:
- $S_p$ = Observed speedup on $p$ processors
- $e$ = Effective serial fraction (estimated)

**Use case:** Diagnose overhead and scalability bottlenecks in real implementations.

---

## Odd-Even Mergesort

### Overview

Odd-Even Mergesort combines two sorted lists by interleaving and comparing pairs of elements.

### Key Idea

When merging two sorted sequences:
- Elements at odd and even indices are separated
- Pairs are compared and exchanged to maintain order
- The first element is the minimum, the last is the maximum

### Algorithm Structure

Given two sorted lists to merge:
1. Take elements at **odd indices** from both lists
2. Take elements at **even indices** from both lists
3. Perform **compare-and-exchange** operations between corresponding pairs
4. Recursively merge smaller lists

### Example

Merge two sorted lists `[a₁, a₂, ..., aₙ]` and `[b₁, b₂, ..., bₙ]`:

```
a = [2, 4, 6, 8]
b = [1, 3, 5, 7]

Odd elements:   a_odd = [2, 6]    b_odd = [1, 5]
Even elements:  a_even = [4, 8]   b_even = [3, 7]

Compare and exchange to produce merged output: [1, 2, 3, 4, 5, 6, 7, 8]
```

### Parallelization Strategy

**Pairs of processors** perform compare-and-exchange operations simultaneously:

- Processor pair (0,1): Compares element at index 0 with element at index 1
- Processor pair (2,3): Compares element at index 2 with element at index 3
- And so on...

### Advantages

- ✓ Highly parallelizable
- ✓ Regular communication pattern
- ✓ Can be implemented with hardware compare-and-exchange units
- ✓ Good for network-based sorting

### Complexity

- **Time:** $O(\log^2 n)$ comparisons
- **Processors:** $O(n)$ required for full parallelization

---

## Bitonic Mergesort

### Bitonic Sequence Definition

A **bitonic sequence** is one with up to two monotonic pieces:
- A sequence increasing then decreasing, OR
- A sequence decreasing then increasing, OR
- Just monotonic increasing or decreasing

### Examples

```
Bitonic sequences:
[1, 3, 5, 7, 6, 4, 2]     (increasing then decreasing)
[6, 4, 2, 1, 3, 5, 7]     (decreasing then increasing - cyclic shift)
[1, 2, 3, 4, 5]           (monotonic increasing)
```

### Key Property of Bitonic Sequences

When we perform a **compare-and-exchange** between elements that are $n/2$ positions apart:

For a bitonic sequence of length $n$:

```
for all i = 0 to n/2-1:
    compare_and_exchange(seq[i], seq[i + n/2])
```

**Result:** Two smaller bitonic sequences where:
- All elements in the left sequence ≤ all elements in the right sequence

### Visualizing Bitonic Sort

```
Original:        [1, 3, 5, 7, 6, 4, 2, 0]   (bitonic)

Step 1: Compare pairs (n/2 apart)
        [1, 3, 2, 0, 6, 4, 5, 7]   (now sorted!)
        
More generally: left ≤ right
        [smaller bitonic] | [larger bitonic]
```

### Bitonic Sorting Algorithm

1. **Build bitonic sequences:** Start with adjacent pairs and recursively build larger bitonic sequences

   ```
   Pairs:     [1,3] [5,7] [6,4] [2,0]   (all bitonic, size 2)
   Next:      [1,3,7,5] [2,0,6,4]       (all bitonic, size 4)
   Final:     [0,1,2,3,4,5,6,7]         (sorted)
   ```

2. **Sort bitonic sequences:** Recursively apply bitonic sort

### Bitonic Sort Structure

```
To sort a sequence of length n:
  1. Recursively build left half into ascending bitonic sequence
  2. Recursively build right half into descending bitonic sequence
  3. These form a combined bitonic sequence
  4. Recursively sort this bitonic sequence
```

### Compare-and-Exchange Networks

Bitonic sort can be expressed as a **sorting network**:
- Gates are compare-and-exchange units
- All comparisons are fixed in advance
- Independent comparisons can happen in parallel

### Advantages

- ✓ **Logarithmic depth:** $O(\log^2 n)$ parallel steps
- ✓ **Highly parallelizable:** Many comparisons in parallel
- ✓ **Sorting network:** Fixed communication pattern
- ✓ **Ideal for SIMD/GPU:** Regular, data-independent operations

### Complexity

- **Sequential:** $O(n \log^2 n)$ comparisons
- **Parallel:** $O(\log^2 n)$ time with $O(n \log^2 n)$ total work
- **Processors:** $O(n)$ required for optimal parallelization

---

## Comparison: Odd-Even vs. Bitonic

| Aspect | Odd-Even | Bitonic |
|--------|----------|---------|
| **Conceptual simplicity** | Simpler | More complex |
| **Parallel depth** | $O(\log^2 n)$ | $O(\log^2 n)$ |
| **Work** | $O(n \log n)$ | $O(n \log^2 n)$ |
| **Network pattern** | Regular pairs | Highly regular |
| **Communication** | Local neighborhoods | Structured pattern |
| **Hardware suitability** | General parallel | SIMD/GPU/network |

---

## Sorting Network Concept

A **sorting network** is:
- A fixed sequence of compare-and-exchange operations
- Independent of the actual data values
- All comparisons determined in advance
- Can exploit massive parallelism

### Example: Bitonic Sort as a Network

For 8 elements, the network has:
- 12 stages
- Up to 4 comparisons per stage
- Each stage takes constant time if comparisons run in parallel

---

## Practical Considerations

### When to Use Each Algorithm

**Odd-Even Mergesort:**
- Merging pre-sorted lists
- When only O(n) processors available
- Distributed memory systems with limited bandwidth

**Bitonic Mergesort:**
- Full sorting of unsorted data
- Many processors available (O(n) or more)
- SIMD architectures
- Sorting networks in hardware

### Overhead Analysis

Both algorithms have overhead from:
1. **Synchronization:** Barrier operations between stages
2. **Communication:** Moving data between processors
3. **Load imbalance:** Not all processors equally busy in all stages

The effective serial fraction (Karp-Flatt metric) quantifies this overhead.

---

## Complexity Summary

| Algorithm | Best Case | Average Case | Worst Case | Parallel Time |
|-----------|-----------|--------------|-----------|----------------|
| Mergesort | $O(n \log n)$ | $O(n \log n)$ | $O(n \log n)$ | $O(\log^2 n)$ |
| Quicksort | $O(n \log n)$ | $O(n \log n)$ | $O(n^2)$ | Variable |
| Bitonic | $O(n \log^2 n)$ | $O(n \log^2 n)$ | $O(n \log^2 n)$ | $O(\log^2 n)$ |
| Odd-Even | $O(n \log n)$ | $O(n \log n)$ | $O(n \log n)$ | $O(\log^2 n)$ |

---

## Key Insights

1. **Regular communication patterns** enable efficient parallelization
2. **Fixed compare-and-exchange operations** allow hardware implementation
3. **Bitonic sequences** have special properties enabling recursive decomposition
4. **Overhead analysis** (Karp-Flatt) reveals practical scalability limits
5. **Algorithm choice** depends on available processors and communication patterns

---

## Next Steps

- Implement bitonic sort on the Pine cluster
- Analyze performance using Amdahl's and Gustafson's laws
- Measure overhead with the Karp-Flatt metric
- Compare against optimized library implementations (e.g., MPI_Sort)

---

## Summary

Bitonic and Odd-Even mergesort are highly parallelizable algorithms with:
- Logarithmic parallel depth
- Highly regular communication patterns
- Suitability for various parallel architectures
- Well-understood performance characteristics

These algorithms exemplify principles of parallel algorithm design: regularity, locality, and minimal overhead.
