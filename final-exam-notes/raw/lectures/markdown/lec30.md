# Lecture 30: Matrix Multiplication I

**Course:** CPSC 375: High-Performance Computing  
**Spring 2026** | Trinity College  
**Instructor:** Prof. Yoon

---

## Overview

Matrix multiplication is a fundamental operation in high-performance computing with direct applications in:
- Linear algebra (systems solving, eigenvalues)
- Machine learning (neural networks)
- Computer graphics (transformations)
- Scientific computing (simulations)

This lecture covers optimization techniques for sequential performance and the foundation for parallel implementations.

---

## Basic Matrix Multiplication

### Problem Statement

Compute $C = A \times B$ where:
- $A$ is $n \times m$
- $B$ is $m \times n$
- $C$ is $n \times n$ (output)

### Standard Algorithm (ijk ordering)

```c
for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
        for (k = 0; k < m; k++) {
            c[i][j] += a[i][k] * b[k][j];
        }
    }
}
```

### Complexity

- **Time:** $O(n^3)$ multiplications and additions
- **Space:** $O(n^2)$ for three $n \times n$ matrices
- **Memory bandwidth:** Critical bottleneck

---

## Loop Ordering Effects

Different orderings of the three loops have **drastically different cache performance**:

### ijk Order (Standard)

```c
for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
        double sum = 0.0;
        for (k = 0; k < n; k++) {
            sum += a[i][k] * b[k][j];  // STRIDE-N ACCESS! ❌
        }
        c[i][j] = sum;
    }
}
```

**Problem:** Inner loop accesses `b[k][j]` with **stride $n$** (column-major access)
- For large matrices exceeding cache size
- Each access brings in a new cache line
- **Cache miss rate:** Very high (~90%)

### kij Order (Improved)

```c
for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
        double factor = a[i][k];
        for (j = 0; j < n; j++) {
            c[i][j] += factor * b[k][j];  // UNIT STRIDE ✓
        }
    }
}
```

**Advantage:** Inner loop accesses `b[k][j]` with **unit stride**
- Sequential memory access
- Maximizes cache utilization
- **Cache miss rate:** Much lower (~5%)

### Performance Comparison

For an $n = 1000$ matrix on typical hardware:

| Order | Time | Speedup |
|-------|------|---------|
| ijk | 2.5s | 1.0× |
| kij | 0.4s | **6.25×** |
| jik | 0.25s | **10×** |

Simply **reordering loops** without changing algorithm gives **6-10× speedup**!

---

## Cache-Based Optimization: Tiling (Blocking)

### Motivation

Even with optimized loop ordering, large matrices don't fit in cache:
- Cache typical: 8 MB - 20 MB per core
- Matrix $n = 10,000$: $10,000^2 \times 8$ bytes = **800 MB** memory

Solution: **Process the matrix in small blocks that fit in cache**

### Tiling Concept

Divide matrices into blocks:
- $B$ = block size (e.g., 64 × 64 or 128 × 128)
- Work on blocks that fit in L3 cache
- Process all operations within a block before moving to next

### Tiled Code (ijk variant)

```c
for (ii = 0; ii < n; ii += block_size) {
    for (jj = 0; jj < n; jj += block_size) {
        for (kk = 0; kk < n; kk += block_size) {
            // Process block of C from [ii:ii+B, jj:jj+B]
            for (i = ii; i < MIN(ii + block_size, n); i++) {
                for (j = jj; j < MIN(jj + block_size, n); j++) {
                    double sum = 0.0;
                    for (k = kk; k < MIN(kk + block_size, n); k++) {
                        sum += a[i][k] * b[k][j];
                    }
                    c[i][j] += sum;
                }
            }
        }
    }
}
```

### How Tiling Improves Cache Performance

**Without tiling:**
- Process entire row of $C$
- Need entire $B$ matrix in cache
- For large $n$, $B$ doesn't fit → many misses

**With tiling:**
- Process $B \times B$ blocks of $C$
- Need $B \times n$ of $A$, $B \times n$ of $B$, and $B \times B$ of $C$
- Much more likely to fit in cache

### Data Reuse Analysis

For a $B \times B$ block of $C$:
- Load $B \times n$ elements from $A$: $O(Bn)$ reads
- Load $B \times n$ elements from $B$: $O(Bn)$ reads
- Perform $B^3$ operations
- **Arithmetic intensity:** $\frac{B^3}{2Bn} = \frac{B}{2}$ operations per memory access

Larger $B$ → better ratio (but limited by cache size)

### Communication Complexity

**Without blocking:**
- Total memory transfers: $\approx O(n^3)$ elements

**With blocking:**
- Total memory transfers: $\approx O(\frac{n^3}{\sqrt{M}})$ elements
- Where $M$ = cache size
- For $n = 1000$, $M = 8$ MB: $O(n^3/2800)$ improvement

---

## Reordering vs. Blocking

### Visualization: 4×4 Matrices with 8-Element Cache

**Matrices:**
```
A = [a00 a01]    B = [b00 b01]    C = [c00 c01]
    [a10 a11]        [b10 b11]        [c10 c11]
```

**Reordering (kij order):**
```
Time 1: Load a[0][0], process row 0 of B
        Cache: [a00, b00, b01, c00, c01, ...]
        
Time 2: Load a[1][0], process row 0 of B  
        Cache: [a10, b00, b01, c00, c01, ...] ← b00, b01 still in cache ✓
        
Time 3: Load a[0][1], process row 1 of B
        Cache: [a01, b10, b11, c00, c01, ...]  ← need new row of B ✗
```

**Blocking (tiling):**
```
Process block C[0:2, 0:2]:
  Load: A[0:2, 0:2], B[0:2, 0:2], C[0:2, 0:2]
  Cache can hold: ~2 blocks × 3 matrices ✓
  
Then process next block independently
```

With blocking, better locality and fewer misses.

---

## Advanced Algorithm: Strassen's Matrix Multiplication

### The Problem with $O(n^3)$

Matrix multiplication is theoretically limited to:
- Standard: $\Omega(n^3)$ operations
- Strassen: $O(n^{2.807})$ operations
- Current best (Alman-Vassilevskii): $O(n^{2.372...})$ operations

### Strassen's Insight

For 2×2 matrices, standard method uses **8 multiplications and 4 additions**.

Strassen showed you can do it with only **7 multiplications at the cost of 18 additions**.

### The Trade-off

Since multiplication is more expensive than addition:
- $1$ multiplication ≈ $k$ additions (typically $k = 2$ to $5$)
- **7 multiplications + 18 additions < 8 multiplications + 4 additions**

### Strassen's Formula

For $C = AB$:

```
m1 = (A[0][0] + A[1][1]) × (B[0][0] + B[1][1])
m2 = (A[1][0] + A[1][1]) × B[0][0]
m3 = A[0][0] × (B[0][1] - B[1][1])
m4 = A[1][1] × (B[1][0] - B[0][0])
m5 = (A[0][0] + A[0][1]) × B[1][1]
m6 = (A[1][0] - A[0][0]) × (B[0][0] + B[0][1])
m7 = (A[0][1] - A[1][1]) × (B[1][0] + B[1][1])

C[0][0] = m1 + m4 - m5 + m7
C[0][1] = m3 + m5
C[1][0] = m2 + m4
C[1][1] = m1 - m2 + m3 + m6
```

### Complexity

- **Time:** $O(n^{\log_2 7}) = O(n^{2.807})$
- **Space:** $O(n^2)$ for temporary matrices
- **Practical:** Slower than standard for small matrices due to overhead

### When Strassen Applies

- Matrices larger than ~500×500
- Multiplication is relatively expensive
- Overhead of temporary allocations acceptable

---

## Summary: Performance Optimization Levels

| Technique | Speedup | Complexity |
|-----------|---------|-----------|
| **Algorithm Choice** | Algorithm dependent | High |
| **Loop Reordering** | 6-10× | Low |
| **Tiling/Blocking** | 5-20× | Medium |
| **Strassen** | $1.5 - 3×$ for huge matrices | Very high |
| **Combined** | 50-100× | Medium-High |

---

## Parallel Considerations

### Data Partitioning Strategies

1. **Block distribution:** Each processor holds $n/\sqrt{p} \times n/\sqrt{p}$ block
2. **Row distribution:** Each processor holds $n/p \times n$ rows
3. **Column distribution:** Each processor holds $n \times n/p$ columns

### Communication Pattern

Naive approach requires $O(n)$ communication per processor per multiply.

Better: **Cannon's algorithm** with $O(n/\sqrt{p})$ communication.

---

## Key Takeaways

1. **Loop ordering matters:** Simple reordering gives 6-10× speedup
2. **Cache is crucial:** Tiling reduces memory traffic by $O(\sqrt{M})$
3. **Block size selection:** Must balance cache utilization and TLB efficiency
4. **Strassen is theoretical:** Practical benefits limited to large matrices
5. **Parallelization:** Matrix structure enables good scalability

---

## Next Lecture

Parallel matrix multiplication with MPI!
