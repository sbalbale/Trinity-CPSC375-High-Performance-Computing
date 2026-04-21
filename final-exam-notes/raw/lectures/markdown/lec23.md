# Lecture 23: The Sieve of Eratosthenes Algorithm

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Parallel Algorithm Case Study - Prime Number Finding

---

## The Sieve of Eratosthenes

### Overview

An ancient algorithm that identifies all prime numbers up to a specified limit by iteratively marking the multiples of each discovered prime as composite.

### Historical Context

- Developed by Eratosthenes (~200 BC)
- One of the oldest and most famous algorithms
- Still widely used despite age
- Perfect example for parallel implementation

---

## Sequential Algorithm

### Algorithm Steps

1. **Create a list** of unmarked natural numbers 2, 3, …, n

2. **Initialize** k ← 2

3. **Repeat until k² > n:**
   - Mark all multiples of k between k² and n
   - Set k ← smallest unmarked number > k

4. **Result:** The unmarked numbers are primes

### Example: Finding Primes Up to 30

**Initial list:**
```
2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
```

**Step 1: k = 2, mark multiples of 2 from 4:**
```
2  3  X  5  X  7  X  9  X  11 X  13 X  15 X  17 X  19 X  21 X  23 X  25 X  27 X  29 X
```

**Step 2: k = 3, mark multiples of 3 from 9:**
```
2  3  X  5  X  7  X  X  X  11 X  13 X  X  X  17 X  19 X  X  X  23 X  25 X  X  X  29 X
```

**Step 3: k = 5, mark multiples of 5 from 25:**
```
2  3  X  5  X  7  X  X  X  11 X  13 X  X  X  17 X  19 X  X  X  23 X  X  X  X  X  29 X
```

**Step 4: k = 7, but 7² = 49 > 30, stop**

**Result - Primes ≤ 30:**
```
2, 3, 5, 7, 11, 13, 17, 19, 23, 29
```

### Visualization

```
Mark all multiples of k    Mark all multiples of k    Mark all multiples of k
(starting from k²)         (starting from k²)         (starting from k²)
k = 2                      k = 3                      k = 5

2  3  4  5  6  7           2  3  5  7  9  11          2  3  5  7  11 13
8  9  10 11 12 13    →     13 15 17 19 21 23   →      17 19 23 25 29
14 15 16 17 18 19          25 27 29                    ...
20 21 22 23 24 25
26 27 28 29 30

X = marked (composite)
unmarked = prime
```

### Time Complexity

**Sequential complexity:** O(n ln (ln n))

- Efficient for moderate n
- Impractical for very large n (e.g., 10⁸)

### Why Impractical for Large n?

1. **Memory:** Need array of size n
   - For n = 10⁸, requires ~100 MB
   - For n = 10⁹, requires ~1 GB

2. **Time:** O(n ln ln n) still significant
   - For n = 10⁸, ~10⁹ operations
   - Can take several seconds

3. **Parallelization Opportunity**
   - Each element independently marked
   - Multiple processors can work on different portions
   - Natural data decomposition

---

## Sources of Parallelism

### Domain Decomposition Approach

```
Data Division:
Original array: [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, ...]

Divided into chunks:
Process 0: [2, 3, 4, 5]
Process 1: [6, 7, 8, 9]
Process 2: [10, 11, 12, 13]
...
```

### Primitive Tasks

- **One primitive task per array element**
- Each task:
  - Maintains mark status (prime/composite)
  - Marks itself when divisible by discovered primes
  - Reports if it remains unmarked (is prime)

### Advantages

1. **Many Tasks**
   - For n = 1000, have 1000 tasks
   - Easy to distribute among p processors
   - Flexibility in load balancing

2. **Independent Operations**
   - Marking one element doesn't affect others
   - No data dependencies between elements
   - Natural parallelism

3. **Scalability**
   - More tasks than processors
   - Works with any number of processors
   - Problem size determines task count

---

## Parallel Algorithm

### Communication Pattern

Two main communication phases:

#### Phase 1: Broadcasting Prime

**Operation:** Broadcast each discovered prime to all processes

**Pattern:**
```
Process 0: "Prime = 2"
            ↓↓↓↓↓
All processes receive k = 2
All mark multiples of 2 in their region
```

#### Phase 2: Finding Next Prime

**Operation:** All processes participate in minimum reduction

**Pattern:**
```
Process 0: Next unmarked = 3
Process 1: Next unmarked = 7
Process 2: Next unmarked = 5
Process 3: Next unmarked = 11
          ↓↓↓↓
        Min reduction
          ↓
        Result: 3
```

### Pseudocode

```
for all processes in parallel:
    Phase 1: Find minimum unmarked number > k
    Phase 2: Broadcast minimum to all
    Phase 3: Mark all multiples of new k in local range
    until k² > n
```

---

## Agglomeration: Data Decomposition Options

### Option 1: Interleaved (Cyclic) Decomposition

```
Process 0: 2,      2+p,    2+2p,    ...
Process 1: 3,      3+p,    3+2p,    ...
Process 2: 4,      4+p,    4+2p,    ...
...
Process p-1: 2+p-1, 2+2p-1, 2+3p-1, ...
```

**Characteristics:**
- Round-robin assignment
- Easy to determine "owner" of each index
- Each process gets scattered elements

**Load Balancing:**
- Good load balance (elements evenly distributed)
- **BUT:** Later in algorithm, most elements marked as composite
- Early primes leave few unmarked elements
- Process handling prime 2 has least work
- Leads to load imbalance

**Example (p=4, n=31):**
```
Process 0: 2, 6, 10, 14, 18, 22, 26, 30 (mostly even, all composite)
Process 1: 3, 7, 11, 15, 19, 23, 27, 31 (3, 7, 11, 19, 23, 31 are primes)
Process 2: 4, 8, 12, 16, 20, 24, 28
Process 3: 5, 9, 13, 17, 21, 25, 29 (5, 13, 17, 29 are primes)

Process 0: All marked (no primes) → no work
Process 1: 6 unmarked → significant work
Process 2: All marked
Process 3: 4 unmarked → significant work
```

### Option 2: Block Decomposition

```
Process 0: 2, 3, 4, 5, 6, ..., ⌈n/p⌉
Process 1: ⌈n/p⌉+1, ..., 2⌈n/p⌉
Process 2: 2⌈n/p⌉+1, ..., 3⌈n/p⌉
...
Process p-1: last block
```

**Characteristics:**
- Consecutive elements in each block
- Equal-sized chunks (approximately)
- More complex owner calculation

**Load Balancing:**
- Better balance than cyclic
- Each process owns contiguous block
- Distributes work more evenly
- Fewer processes idle late in algorithm

**Advantages:**
1. **Better Locality**
   - Cache performance improved
   - Memory access patterns better
   - Fewer cache misses

2. **Balanced Workload**
   - All processes have equal initial work
   - Distribution remains relatively balanced
   - Fewer idle processes

3. **Simpler Communication**
   - Fewer messages between neighboring processes
   - Better network locality
   - Easier to optimize

---

## Block Decomposition Implementation

### Macros for Block Calculation

```c
#define BLOCK_LOW(id, p, n) \
    ((id) * (n) / (p))

#define BLOCK_HIGH(id, p, n) \
    (BLOCK_LOW((id)+1, p, n) - 1)

#define BLOCK_SIZE(id, p, n) \
    (BLOCK_LOW((id)+1, p, n) - BLOCK_LOW((id), p, n))

#define BLOCK_OWNER(index, p, n) \
    (((p) * (index) + 1) - 1) / (n))
```

### Examples

**Given:** n = 10, p = 4

```
BLOCK_LOW(0, 4, 10) = 0*10/4 = 0 (element 2)
BLOCK_LOW(1, 4, 10) = 1*10/4 = 2 (element 4)
BLOCK_LOW(2, 4, 10) = 2*10/4 = 5 (element 7)
BLOCK_LOW(3, 4, 10) = 3*10/4 = 7 (element 9)

Process 0: indices 0-2 → elements 2, 3, 4
Process 1: indices 2-4 → elements 4, 5, 6
Process 2: indices 5-6 → elements 7, 8
Process 3: indices 7-9 → elements 9, 10, 11
```

---

## Agglomeration Goals

### Goal 1: Consolidate Tasks

**Before:** One task per element (thousands of tasks)
**After:** One agglomerated task per processor

Benefits:
- Reduces scheduling overhead
- Fewer context switches
- Simplifies implementation

### Goal 2: Reduce Communication Cost

**Strategy:** Eliminate communication between primitive tasks in same agglomerated task

```
Before:
Task 1 → Send to Task 2
Task 1 → Send to Task 3
Task 1 → Send to Task 4

After:
Agglomerated Task 1 → Single send to Agglomerated Task 2
(internal operations no longer require messaging)
```

### Goal 3: Balance Computation

Ensure:
- All processes have similar workload
- No process is significantly slower
- Idle time is minimized
- Scalability is maintained

**Metric:** Workload balance ratio
- Ideal: All processes equal work
- Reality: Some imbalance acceptable
- Monitor to detect issues

---

## Even Integer Optimization

### Insight

Even integers (except 2) are never prime!

### Optimization

**Skip even numbers after 2:**

```
Original: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, ...

Optimized: 2, 3, 5, 7, 9, 11, 13, ...
(skip 4, 6, 8, 10, 12, ...)
```

### Benefits

1. **Reduced Array Size**
   - Only store odd numbers
   - Array size roughly n/2
   - Memory usage cut in half

2. **Fewer Operations**
   - Only process half the elements
   - Marking operations reduced
   - Computation time reduced significantly

3. **Implementation Consideration**
   - Index mapping more complex
   - Must translate between index and odd number
   - Worth the optimization effort

### Index Mapping

```c
// Convert index to actual number
#define INDEX_TO_NUMBER(index) (2 * (index) + 3)

// Convert number to index
#define NUMBER_TO_INDEX(number) ((number) - 3) / 2
```

---

## Eliminating Broadcast

### Problem with Broadcasting Primes

Each discovered prime must be sent to all processes.

### Solution: Local Prime Discovery

**Idea:** Each process finds its own next prime locally

- Replicate the computation of primes locally
- Each process computes same sieving primes
- Eliminates need for broadcast

### Trade-off

**Pros:**
- Eliminates MPI_Bcast operation
- Each process independent
- No synchronization needed for prime generation

**Cons:**
- Redundant computation (computation done p times)
- Each process computes same primes
- Works well if:
  - Sieving prime computation is small
  - Number of primes to generate is modest (√n primes)

### When Worthwhile

For √n primes to compute:
- n = 10⁸ → √n ≈ 10⁴ primes
- Redundant computation: 10⁴ × p operations
- Broadcast saves: p × (communication overhead)
- Typically redundant computation is worth it

---

## Cache Efficiency

### Problem

Each process is marking widely dispersed elements of a very large array.

**Example:**
```
Process 0 marks: indices 0, 2, 4, 6, ... (multiples of 2)
                 indices 0, 9, 18, 27, ... (multiples of 3)
                 indices 0, 25, 50, 75, ... (multiples of 5)

These are scattered throughout memory!
```

### Cache Issues

- Poor cache hit rate
- Memory access pattern: random
- Cache line misses frequent
- Performance significantly degraded

### Reorganizing Loops

**Original:**
```c
for (j = 0; j < size; j++) {
    if (!marked[j])
        continue; // Skip unmarked (or mark here)
}
```

**Reorganized:**
```c
// For each prime k
for (i = first_multiple; i < size; i += prime) {
    marked[i] = 1; // Sequential access pattern
}
```

### Benefits

1. **Sequential Access**
   - Predictable memory pattern
   - Cache prefetcher effective
   - Much higher cache hit rate

2. **Performance Improvement**
   - Can be 10-100x faster
   - Dominates overall execution time
   - Critical optimization

---

## Summary

### Algorithm Characteristics

1. **Natural Parallelism**
   - Data independent
   - Domain decomposition applicable
   - Scalable to many processors

2. **Communication Patterns**
   - Collective operations (broadcast, reduce)
   - Structured communication
   - Suitable for MPI

3. **Load Balance Considerations**
   - Block decomposition better than cyclic
   - Work decreases over time
   - Optimization critical

4. **Optimizations**
   - Skip even numbers (50% reduction)
   - Local prime computation (eliminate broadcast)
   - Memory access reorganization (cache efficiency)

### Performance Lessons

1. **Premature optimization avoidance**
   - Start with simple parallel implementation
   - Profile to find actual bottlenecks
   - Optimize what matters

2. **Algorithm selection**
   - Domain decomposition vs. Cyclic
   - Trade-offs in complexity vs. performance
   - Scalability considerations

3. **Hardware awareness**
   - Cache behavior matters
   - Memory access patterns important
   - Profiling essential

---

## References

- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
- Pacheco, P. S. (2011). An Introduction to Parallel Programming
