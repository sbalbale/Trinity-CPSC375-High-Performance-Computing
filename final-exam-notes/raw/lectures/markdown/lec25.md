# Lecture 25: Parallel Sorting Algorithms

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Parallelizing Comparison-Based Sorting

---

## Overview: Sorting Complexity

### Sequential Sorting

**Best Known Bound:** O(n log n)
- Optimal for comparison-based sorting
- Cannot sort faster without additional information about data

**Examples of O(n log n) algorithms:**
- Quicksort (average case)
- Mergesort
- Heapsort

### Potential Parallel Speedup

**With p Processors:**

Best we can expect based on sequential algorithm:

```
O(n log n) / p = O(log n) if p = n
```

**Interpretation:**
- If we have n processors
- And O(n log n) is the lower bound
- Best parallel time is O(log n)

### Reality Check

**Practical Considerations:**
- This theoretical bound is rarely achieved
- Constant factors hidden in O-notation are extremely large
- Communication overhead significant
- Load balancing difficult
- Memory bandwidth becomes bottleneck

**Why So Hard?**
1. Multiple comparisons needed
2. Many interdependencies
3. Communication cost high
4. Achieving ideal speedup requires fine-tuning

---

## Compare-and-Exchange Sorting Algorithms

### Concept

Form the basis of several classical sequential sorting algorithms.

### Basic Operation

Compare two numbers A and B, and exchange if necessary:

```c
if (A > B) {
    temp = A;
    A = B;
    B = temp;
}
```

After exchange:
- A contains the smaller value
- B contains the larger value

### Examples

1. **Bubble Sort**
   - Repeatedly compare adjacent pairs
   - Smallest values "bubble" to top
   - O(n²) time complexity

2. **Odd-Even Sort** (Brick Sort)
   - Compare pairs at even and odd positions
   - Alternates which pairs compare
   - Parallelizable (all even pairs simultaneously)

3. **Bitonic Sort**
   - Recursively defined
   - Good for p = 2^k processors
   - O(log² n) parallel time

4. **AKS Network**
   - Optimal O(log n) depth
   - Constant factors prohibitively large

---

## Message-Passing Compare-and-Exchange

### Scenario

**Two Processors P1 and P2**

P1 has value A
P2 has value B

Goal: P1 ends up with min(A,B), P2 ends up with max(A,B)

### Approach 1: Sequential Exchange

**Steps:**

```
1. P1 sends A to P2
   P1: (waits for B)
   P2: compares A and B

2. If A > B, P2 sends B to P1
   Else P2 sends A to P1
   
3. P1 receives the smaller value
```

**Code:**

**Process P1:**
```c
send(&A, P2);
recv(&A, P2);
// A now contains min(A_initial, B)
```

**Process P2:**
```c
recv(&A, P1);
if (A > B) {
    send(&B, P1);
    B = A;  // B = max(A_initial, B_initial)
} else {
    send(&A, P1);
    // B = B_initial (already max)
}
```

### Approach 2: Simultaneous Exchange

**Better Approach:** Exchange happens concurrently

**Steps:**

```
1. P1 sends A to P2
   P2 sends B to P1
   (Both happen simultaneously)

2. P1 receives B from P2
   P1 compares A and B
   P1 keeps smaller value

3. P2 receives A from P1
   P2 compares A and B
   P2 keeps larger value
```

**Code:**

**Process P1:**
```c
send(&A, P2);
recv(&B, P2);
if (A > B)
    A = B;  // A = min(A_initial, B)
```

**Process P2:**
```c
recv(&A, P1);
send(&B, P1);
if (A > B)
    B = A;  // B = max(A_initial, B)
```

### Advantages of Approach 2

1. **Parallelism**
   - Both send and receive happen concurrently
   - Less total time

2. **Simplicity**
   - Both processes do similar work
   - No conditional branching in messaging

3. **Symmetry**
   - Elegant algorithm structure
   - Easy to generalize

---

## Message-Passing Compare-and-Exchange Details

### Execution Diagram

```
        P1              P2
        
Initial: A=50          B=30
          │              │
          │   Send(A)    │
          ├─────────────→│
          │              │
          │   Send(B)    │
          │←─────────────┤
          │              │
     Compare          Compare
     A=50, B=30      A=50, B=30
          │              │
          ▼              ▼
        A=30             B=50
        (min)           (max)
```

### Code Implementation

**Both Processes Execute:**

```c
// Process P1
send(&A, P2);
recv(&B, P2);
if (A > B)
    A = B;

// Process P2
recv(&A, P1);
send(&B, P1);
if (A > B)
    B = A;
```

### Floating-Point Precision Issue

**Important Consideration:**

```c
if (A > B) { ... }
```

This condition on P1 and P2 must return the same Boolean result.

**Problem:** Floating-Point Arithmetic
- Different processors may operate at different precisions
- Floating-point rounding varies
- If slightly different results, comparison might differ
- Leads to inconsistent sort results

**Solution:**
1. Use integer comparison when possible
2. Ensure consistent rounding modes
3. Understand this limitation
4. In practice: Usually doesn't occur with reasonable data

**General Principle:**

When duplicating computations across processors to reduce communication:
- Must ensure identical results on all processors
- Floating-point arithmetic may vary slightly
- Need careful handling of precision

---

## Odd-Even Sort (Brick Sort)

### Algorithm Concept

Sort using compare-and-exchange with specific pairing patterns.

### Sequential Steps

1. **Odd Phase:** Compare/exchange (1,2), (3,4), (5,6), ...
   ```
   [5,4,3,2,1] → [4,5,2,3,1] after odd phase
   ```

2. **Even Phase:** Compare/exchange (2,3), (4,5), (6,7), ...
   ```
   [4,5,2,3,1] → [4,2,5,1,3] after even phase
   ```

3. **Repeat:** Odd, even, odd, even, ... until sorted

### Parallelization

**Key Insight:** All odd-phase pairs can work simultaneously!

```
Odd Phase (Parallel):
[P0,P1] [P2,P3] [P4,P5] [P6,P7]  ← Can execute in parallel

Even Phase (Parallel):
    [P1,P2] [P3,P4] [P5,P6]     ← All in parallel
```

### Time Complexity

- **Sequential:** O(n²) operations with n processors
- **Parallel:** O(n) phases, each O(1) time
- **Total:** O(n) parallel time with n processors

### Algorithm Structure

```
Odd-Even Sort with p processors:

for phase = 1 to n:
    if phase is odd:
        // All odd pairs compare simultaneously
        for i = 0 to p-2 step 2:  // Parallel loop
            compare_exchange(Process[i], Process[i+1])
    else:
        // All even pairs compare simultaneously
        for i = 1 to p-1 step 2:  // Parallel loop
            compare_exchange(Process[i], Process[i+1])
```

### Visual Example (8 Elements, 4 Processors)

```
Initial:   [7,5,3,1] [6,4,2,0]  (processor pairs)
                 ↓
After 1    [5,7,1,3] [4,6,0,2]
          (odd pairs swapped)
                 ↓
After 2    [5,1,7,3] [4,0,6,2]
          (even pairs swapped)
                 ↓
... continue until sorted
```

---

## Scaling Considerations

### Message Overhead

**Cost Analysis:**
- Sorting is communication-intensive
- Every comparison might require message
- Message overhead can dominate
- Bandwidth limited

### Processor Count

**Sweet Spot:**
- Too few: Under-utilize parallelism
- Too many: Communication overhead dominates
- Often: p ≈ √n is practical maximum

### Network Topology

**Implications:**
- Nearest-neighbor communication (adjacent processors)
- Benefits from linear or 2D mesh topology
- Minimizes communication distance
- Better than fully-connected for some networks

---

## Bitonic Sort

### Concept

A sorting network based on recursively building bitonic sequences.

### Bitonic Sequence

A sequence that:
1. Increases then decreases, OR
2. Decreases then increases

**Examples:**
```
[1, 3, 5, 7, 6, 4, 2]     (increasing then decreasing)
[7, 5, 3, 1, 2, 4, 6]     (decreasing then increasing)
[1, 2, 3, 4, 5, 6, 7]     (monotonic is bitonic)
```

### Recursive Structure

Building blocks:
1. **Base case:** Single element is bitonic
2. **Recursive step:** Combine two bitonic sequences

### Complexity

- **Best for p = 2^k processors**
- **Parallel time:** O(log² n)
- **Comparators:** O(n log² n)

### Advantages

1. Optimal parallel time (for networks)
2. Fully parallelizable
3. Scales well to power-of-2 processors

### Disadvantages

1. More complex to implement
2. Overhead in practice
3. Requires special processor count

---

## AKS Network

### Overview

Optimal depth O(log n) sorting network.

### Theoretical Significance

- Proves sorting network with O(log n) depth exists
- Settles long-standing theory question
- Only O(n log n) total comparators

### Practical Limitations

**NOT used in practice because:**

1. **Hidden Constants**
   - Constant in O(log n) is extremely large
   - Actual time: months for modest data

2. **Complexity**
   - Sorting network description complicated
   - Hard to implement correctly
   - High overhead in practice

3. **Comparison-Based**
   - Still requires comparisons
   - Not fundamentally faster than other methods

### Lesson

- Theoretical optimal ≠ Practical optimal
- Hidden constants matter
- Simple algorithms often better in practice

---

## Comparing Parallel Sorting Approaches

### Approach Comparison

| Algorithm | Depth | Comparators | Practicality | Scaling |
|-----------|-------|-------------|-------------|---------|
| Odd-Even | O(n) | O(n²) | Excellent | p = n |
| Bitonic | O(log² n) | O(n log² n) | Good | p = 2^k |
| AKS | O(log n) | O(n log n) | Poor | Theory only |
| Quicksort | - | O(n log n) avg | Good | Limited |
| Mergesort | - | O(n log n) | Fair | Better scaling |

---

## Communication vs. Computation

### Critical Issue in Parallel Sorting

**Balance:**
```
Computation: Each comparison is O(1)
Communication: Message sending is O(1000s)
```

**Result:** Communication dominates

### Mitigation Strategies

1. **Large Data Items**
   - Each comparison involves large objects
   - Amortizes message overhead

2. **Clever Scheduling**
   - Group comparisons
   - Batch communication

3. **Network Optimization**
   - Use efficient networks
   - Minimize hops

4. **Hybrid Approaches**
   - Local sorting with sequential algorithm
   - Limited parallel communication
   - Better balance

---

## Practical Implementation

### Real-World Approaches

1. **Sample Sort**
   - Sample subset to find partitions
   - Local sort on each processor
   - Minimal global communication

2. **Coarse-Grained Algorithms**
   - Each processor sorts locally
   - Exchange with neighbors
   - Reduce communication frequency

3. **GPU Sorting**
   - Use GPU for local sorting
   - CPU handles communication
   - Leverage strengths of each

### Current Practice

Most high-performance systems use:
- Hybrid algorithms
- Adaptive methods
- Problem-specific optimization
- Not pure comparison-sort networks

---

## Summary

### Key Concepts

1. **Comparison-Based Lower Bound**
   - O(n log n) sequential time
   - Best parallel: O(n/p + log n) ideally
   - Communication overhead dominates

2. **Compare-and-Exchange Paradigm**
   - Simple basic operation
   - Hard to achieve high efficiency
   - Message overhead critical

3. **Algorithm Families**
   - **Odd-Even Sort:** O(n) parallel, simple, scalable
   - **Bitonic Sort:** O(log² n) parallel, complex, power-of-2
   - **AKS Network:** O(log n) parallel, impractical theoretically

4. **Practical Considerations**
   - Communication costs dominate
   - Processor count limited
   - Hybrid approaches preferred
   - Problem-specific tuning essential

### When to Use Parallel Sorting

- **Worth it for:** Large datasets, many processors
- **Avoid for:** Small datasets, limited processors
- **Modern approach:** Use library implementations
  - Highly optimized
  - Adaptive to problem size
  - Handle edge cases

### Lessons Learned

1. **Theory vs. Practice**
   - Optimal theoretical ≠ optimal practical
   - Constants matter
   - Real constraints (network, memory) important

2. **Communication is Costly**
   - Often dominates computation
   - Minimize message frequency
   - Batch operations when possible

3. **Algorithm Selection**
   - Problem-dependent
   - Data size matters
   - Hardware characteristics matter

4. **Profiling is Essential**
   - Measure actual performance
   - Identify true bottlenecks
   - Validate optimizations

---

## References

- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). Introduction to Algorithms
- Cole, R. (1988). Parallel Merge Sort. SIAM Journal on Computing
- Ajtai, M., Komlós, J., & Szemerédi, E. (1983). Sorting in c log n parallel steps
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
- OpenMPI Documentation: https://www.open-mpi.org/doc/
- Pacheco, P. S. (2011). An Introduction to Parallel Programming
