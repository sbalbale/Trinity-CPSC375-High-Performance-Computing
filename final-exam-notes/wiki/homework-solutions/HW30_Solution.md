# Homework 30 Solution
**CPSC 375: High-Performance Computing — Spring 2026**

---

## Problem 1

**Problem Statement:** Consider the iterative, row-oriented algorithm (the `ijk` version). Explain why performance (in megaflops) drops sharply once the matrix size exceeds 100–200, as shown in the graph on slide 4. Also, define *stride* in the context of accessing B in this algorithm. How does a large stride specifically impact the CPU cache and the number of memory transfers required?

### Solution

**Why performance drops sharply:**

In the `ijk` loop order, computing a single row of C requires iterating over every element of matrix B. Elements of A are accessed sequentially (good spatial locality), but elements of B are accessed **column by column** — i.e., `B[0][j], B[1][j], B[2][j], ...` — which are `n` elements apart in memory (row-major storage). For small matrices (n < 100–200), the entire matrix B fits within the L1 or L2 cache, so these non-sequential accesses are still fast. Once n exceeds this threshold, B no longer fits in cache. Every access to a column element of B causes a **cache miss**, forcing a slow main-memory fetch. Performance collapses because the algorithm becomes memory-bandwidth bound rather than compute-bound.

**Definition of stride:**

Stride is the number of memory locations between consecutively accessed elements. In the `ijk` version, when we access `B[k][j]` for increasing values of `k` (fixed `j`), consecutive accesses are `n` elements apart. The stride of accessing B in the `ijk` version is therefore **n** (the number of columns).

**Impact of large stride:**

A CPU cache line typically holds 8–16 consecutive doubles. When stride = n >> 1, each access to `B[k][j]` falls on a different cache line. The cache cannot reuse the loaded line before it is evicted. This means:
- Nearly every access to B results in a **cache miss**.
- The number of memory transfers scales as O(n²) just for the B accesses per row of C, and O(n³) overall.
- The CPU stalls waiting for data from main memory rather than executing floating-point operations, tanking megaflop throughput.

---

## Problem 2

**Problem Statement:** Loop reordering introduces the `ikj` version of matrix multiplication. Provide the specific line of code from the `ikj` version that demonstrates spatial locality for both matrix C and matrix B. Explain why the variable `r` is used to store `a[i][k]` outside of the innermost loop. How many times is this specific element of A reused before the next value of `k` is loaded?

### Solution

The `ikj` version:

```c
for (i = 0; i < n; i++) {
    for (k = 0; k < n; k++) {
        r = a[i][k];
        for (j = 0; j < n; j++)
            c[i][j] += r * b[k][j];   // ← This line
    }
}
```

**Line demonstrating spatial locality:**

```c
c[i][j] += r * b[k][j];
```

In this innermost loop, `j` is the innermost index. Both `c[i][j]` and `b[k][j]` are accessed with **stride 1** — consecutive elements along row `i` of C and row `k` of B. Since both arrays are stored in row-major order, these are adjacent memory locations, so the CPU loads a cache line and uses every word in it before fetching the next. This is excellent spatial locality.

**Why `r = a[i][k]` is hoisted outside the inner loop:**

In the inner loop over `j`, the indices `i` and `k` are both fixed. `a[i][k]` is the same value for all `n` iterations of the `j` loop. By storing it in the scalar variable `r` before the loop, we:
1. Avoid re-loading `a[i][k]` from memory (or cache) on every iteration.
2. Allow the compiler/CPU to keep `r` in a **register**, the fastest storage available.
3. Make it explicit that this value never changes in the inner loop, enabling further compiler optimizations.

**How many times is `a[i][k]` reused?**

The element `a[i][k]` is used once per iteration of the innermost `j` loop, which runs from `j = 0` to `j = n - 1`. Therefore, it is reused **n times** before the outer `k` loop advances to the next value of `k` and loads a new `a[i][k]`.

---

## Problem 3

**Problem Statement:** Tiling (blocking) is used to manage cache capacity. Given a cache size M, what is the communication complexity (number of memory transfers) for blocked matrix multiplication compared to the standard approach? In addition, tiling uses a `MIN` function. Explain the practical purpose of this function when the matrix size n is not perfectly divisible by the `block_size`.

### Solution

**Communication complexity comparison:**

| Approach       | Memory Transfer Complexity |
|----------------|---------------------------|
| Standard (no blocking) | O(n³)             |
| Blocked (tiling)       | O(n³ / √M)        |

where M is the size of the cache in elements.

The standard `ijk` approach has O(n³) memory transfers because B is accessed with stride n and nearly every access is a cache miss for large n.

With blocking, we choose a block size b ≈ √M such that three b×b submatrices (one tile of A, B, and C) fit simultaneously in cache. Each tile is loaded from memory once and reused for all b multiply-accumulate operations involving it, before being replaced. This temporal reuse reduces the total number of memory transfers by a factor of √M compared to the naive approach.

**Purpose of the `MIN` function:**

The tiled loop iterates in steps of `block_size`:

```c
for (i = ii; i < MIN(ii + block_size, n); i++)
for (j = jj; j < MIN(jj + block_size, n); j++)
for (k = kk; k < MIN(kk + block_size, n); k++)
```

When `n` is not evenly divisible by `block_size`, the last tile in each dimension is **smaller than a full block**. For example, if n = 10 and block_size = 4, the last tile covers indices 8 and 9 only. Without `MIN`, the inner loops would run past index `n - 1`, causing **out-of-bounds memory accesses**. The `MIN(ii + block_size, n)` clamps the loop bound to `n`, ensuring the final partial tile is handled correctly without accessing memory outside the matrix.

---

## Problem 4

**Problem Statement:** Strassen's algorithm improves matrix multiplication efficiency by reducing the number of multiplications required for a 2×2 block from eight to seven.

### Part A — Computing C₁₂

**Problem Statement:** Use the specific formulas provided on slides 16 and 17 to calculate the value of the quadrant C₁₂ given only the intermediate products P1 through P7, noting that the quadrant is defined as C₁₂ = P₁ + P₂.

**From slide 17**, the seven intermediate products are:

```
P₁ = A₁₁(B₁₂ - B₂₂)
P₂ = (A₁₁ + A₁₂)B₂₂
P₃ = (A₂₁ + A₂₂)B₁₁
P₄ = A₂₂(B₂₁ - B₁₁)
P₅ = (A₁₁ + A₂₂)(B₁₁ + B₂₂)
P₆ = (A₁₂ - A₂₂)(B₂₁ + B₂₂)
P₇ = (A₁₁ - A₂₁)(B₁₁ + B₁₂)
```

**From slide 17**, the formula for C₁₂ is:

```
C₁₂ = P₁ + P₂
```

**Expanding by substitution:**

```
C₁₂ = P₁ + P₂
     = A₁₁(B₁₂ - B₂₂) + (A₁₁ + A₁₂)B₂₂
     = A₁₁·B₁₂ - A₁₁·B₂₂ + A₁₁·B₂₂ + A₁₂·B₂₂
     = A₁₁·B₁₂ + A₁₂·B₂₂
```

**Verification:** This matches the standard formula for the upper-right quadrant of the product C = AB:

```
C₁₂ = A₁₁·B₁₂ + A₁₂·B₂₂  ✓
```

So `C₁₂ = P₁ + P₂` correctly computes the upper-right block using only 2 of the 7 precomputed products, with no additional multiplications.

---

### Part B — Implementing Strassen's Algorithm

**Problem Statement:** Implement Strassen's algorithm using a divide-and-conquer approach that recursively splits n×n matrices into four n/2×n/2 sub-matrices until the base case is reached.

```python
import numpy as np

def strassen(A, B):
    """
    Multiply two n-by-n matrices using Strassen's algorithm.
    Assumes n is a power of 2.
    Time complexity: O(n^2.81)
    """
    n = A.shape[0]

    # Base case: 1x1 matrix — use scalar multiplication
    if n == 1:
        return A * B

    # Divide each matrix into four n/2 x n/2 submatrices
    mid = n // 2
    A11 = A[:mid, :mid]
    A12 = A[:mid, mid:]
    A21 = A[mid:, :mid]
    A22 = A[mid:, mid:]

    B11 = B[:mid, :mid]
    B12 = B[:mid, mid:]
    B21 = B[mid:, :mid]
    B22 = B[mid:, mid:]

    # Compute the 7 intermediate products recursively (7 multiplications)
    P1 = strassen(A11,       B12 - B22)
    P2 = strassen(A11 + A12, B22      )
    P3 = strassen(A21 + A22, B11      )
    P4 = strassen(A22,       B21 - B11)
    P5 = strassen(A11 + A22, B11 + B22)
    P6 = strassen(A12 - A22, B21 + B22)
    P7 = strassen(A11 - A21, B11 + B12)

    # Combine the 7 products into the four quadrants of C (additions only)
    C11 = P5 + P4 - P2 + P6
    C12 = P1 + P2
    C21 = P3 + P4
    C22 = P1 + P5 - P3 - P7

    # Assemble result matrix from four quadrants
    C = np.zeros((n, n))
    C[:mid, :mid] = C11
    C[:mid, mid:] = C12
    C[mid:, :mid] = C21
    C[mid:, mid:] = C22

    return C


# --- Example / Verification ---
A = np.array([[1, 2],
              [3, 4]], dtype=float)

B = np.array([[5, 6],
              [7, 8]], dtype=float)

C_strassen = strassen(A, B)
C_standard = A @ B

print("Strassen result:")
print(C_strassen)
print("\nStandard result:")
print(C_standard)
print("\nResults match:", np.allclose(C_strassen, C_standard))
```

**Output:**
```
Strassen result:
[[19. 22.]
 [43. 50.]]

Standard result:
[[19. 22.]
 [43. 50.]]

Results match: True
```

**Complexity Justification:**

At each level of recursion, we make **7** recursive calls on matrices of size n/2 (instead of 8 in the naive divide-and-conquer). By the Master Theorem:

```
T(n) = 7·T(n/2) + O(n²)
     = O(n^(log₂7))
     ≈ O(n^2.807)
```

This beats the standard O(n³) for large n.

---

## Problem 5

**Problem Statement:** Compare the two optimization techniques discussed on slide 12. If a matrix is significantly larger than the cache, why might loop reordering alone fail to reach peak performance? Which type of locality (spatial or temporal) is specifically improved by blocking that is not explicitly managed by simple reordering? Refer to the diagrams on slide 13 to support your answer.

### Solution

**Loop Reordering (ikj version):**

Loop reordering (e.g., switching from `ijk` to `ikj`) eliminates the stride-n bottleneck when accessing B. In the `ikj` order, both `b[k][j]` and `c[i][j]` are accessed with stride 1 (along a row), achieving excellent **spatial locality**. This allows the CPU to fully utilize each loaded cache line.

**Why reordering alone fails for large matrices:**

As noted on slide 12, loop reordering does not explicitly manage the **capacity** of the cache. Even with stride-1 access, the CPU loads entire rows of B and C into cache. When the matrix is significantly larger than the cache, as the outer loops advance, rows that were recently loaded get evicted before they can be reused in subsequent outer-loop iterations. Specifically, when `i` advances to the next row and `k` restarts from 0, rows of B that were loaded for the previous `i` are no longer in cache. They must be re-fetched from main memory for every new value of `i` — leading to O(n³) total memory transfers, just like the naive approach.

**What blocking adds — temporal locality:**

As illustrated in the slide 13 diagram:

- **Reordering** achieves *spatial locality*: elements are accessed sequentially along a row (stride-1 pattern on B, shown by the row-arrow access pattern in the top-right diagram).
- **Blocking** additionally achieves *temporal locality*: by working on a b×b tile at a time, the same tile of B (and A) is reused for all elements in the corresponding tile of C **before** being evicted from cache. The bottom-right diagram shows a 2×2 block of B (marked with "x") being accessed for all combinations of the corresponding A block rows and C block columns — the same cached data is reused multiple times without re-loading.

In summary: reordering fixes *how* data is accessed (stride), while blocking fixes *how long* data stays useful in cache before being replaced. Blocking explicitly manages cache **capacity**, reducing the number of memory transfers from O(n³) to O(n³/√M).
