---
tags: [#homework/solutions, #exam/algorithm, #exam/theory]
sources: [Homework 30.pdf]
created: 2026-04-21
updated: 2026-04-21
aliases: [HW30]
---

# Homework 30 Solutions

>[!abstract]
> Solutions to Homework 30 covering matrix multiplication optimization, including loop reordering, tiling (blocking), and Strassen's Algorithm.

## 1. The $ijk$ Algorithm and Stride

>[!example] $ijk$ Matrix Multiplication
> **Question:** Explain why performance drops sharply when matrix size exceeds 100-200. Define stride in the context of accessing $B$ in this algorithm. How does a large stride specifically impact the CPU cache and the number of memory transfers?
> 
> **Solution:**
> - **Performance Drop:** Performance drops due to **cache capacity misses**. As the matrix grows beyond the cache size, the working set can no longer fit in the L1/L2 cache, leading to frequent evictions of data that will be needed again shortly.
> - **Stride Definition:** Stride is the distance in memory between consecutive data accesses. In the $ijk$ algorithm, the innermost loop iterates over $k$. For matrix $B$, accessed as `B[k][j]`, changing $k$ accesses different rows in the same column. Thus, the stride is equal to the size of a row.
> - **Impact:** A large stride destroys spatial locality. Each memory access fetches a full cache line, but only one element is used before the next cache line is fetched. This causes cache thrashing, evicting useful data prematurely and drastically increasing the number of memory transfers (cache misses) between RAM and the CPU cache.

## 2. Loop Reordering ($ikj$)

>[!example] $ikj$ Loop Reordering
> **Question:** Provide the specific line of code from the $ikj$ version that demonstrates spatial locality for both matrix $C$ and matrix $B$. Explain why the variable $r$ is used to store `A[i][k]` outside of the innermost loop. How many times is this specific element of $A$ reused before the next value of $k$ is loaded?
> 
> **Solution:**
> - **Code Line:** `C[i][j] += r * B[k][j];`
> - **Explanation:** In the $ikj$ version, the innermost loop index is $j$. Both $C$ (`C[i][j]`) and $B$ (`B[k][j]`) are accessed sequentially along the row as $j$ increments, maximizing spatial locality and cache line utilization.
> - **Variable $r$:** The value `A[i][k]` only depends on $i$ and $k$. By storing it in a register variable $r$ outside the innermost $j$ loop, we avoid redundantly fetching the same value from memory $n$ times during the innermost loop execution.
> - **Reuse:** The element `A[i][k]` is reused **$n$ times** (where $n$ is the row size).

## 3. Tiling (Blocking)

>[!example] Tiling Cache Optimization
> **Question:** Given a cache size $M$, what is the communication complexity for blocked matrix multiplication compared to the standard approach? Explain the practical purpose of the `MIN` function when the matrix size $n$ is not perfectly divisible by the `block_size`.
> 
> **Solution:**
> - **Communication Complexity:** Standard matrix multiplication requires $O(n^3)$ memory transfers. Blocked matrix multiplication reduces this by leveraging temporal locality, resulting in a communication complexity of **$O(n^3 / \sqrt{M})$**.
> - **`MIN` Function Purpose:** When the matrix size is not a perfect multiple of the block size, the final blocks at the edges will be smaller than `block_size`. The `MIN` function ensures the loop boundaries correctly truncate at the matrix edge, preventing out-of-bounds memory access errors.

## 4. Strassen's Algorithm

>[!example] Strassen's Algorithm Calculation
> **Question:** A. Use the specific formulas to calculate the value of the quadrant $C_{12}$ given $P_1$ through $P_7$. B. Implement Strassen's algorithm using a divide-and-conquer approach.
> 
> **Solution (A):**
> Given formulas:
> $P_1 = A_{11} \times (B_{12} - B_{22})$
> $P_2 = (A_{11} + A_{12}) \times B_{22}$
> 
> Calculate $C_{12}$:
> $$C_{12} = P_1 + P_2$$
> $$C_{12} = (A_{11}B_{12} - A_{11}B_{22}) + (A_{11}B_{22} + A_{12}B_{22})$$
> $$C_{12} = A_{11}B_{12} + A_{12}B_{22}$$
> This correctly matches the standard definition for the top-right quadrant of the result matrix.
> 
> **Solution (B):**
> >[!code] Strassen Divide and Conquer
> >```text
> >function Strassen(A, B, n):
> >    if n == 1:
> >        return A * B
> >    
> >    // Split matrices into quadrants of size n/2 x n/2
> >    A11, A12, A21, A22 = Split(A)
> >    B11, B12, B21, B22 = Split(B)
> >    
> >    // Calculate 7 products recursively
> >    P1 = Strassen(A11, B12 - B22, n/2)
> >    P2 = Strassen(A11 + A12, B22, n/2)
> >    P3 = Strassen(A21 + A22, B11, n/2)
> >    P4 = Strassen(A22, B21 - B11, n/2)
> >    P5 = Strassen(A11 + A22, B11 + B22, n/2)
> >    P6 = Strassen(A12 - A22, B21 + B22, n/2)
> >    P7 = Strassen(A11 - A21, B11 + B12, n/2)
> >    
> >    // Combine into final quadrants
> >    C11 = P5 + P4 - P2 + P6
> >    C12 = P1 + P2
> >    C21 = P3 + P4
> >    C22 = P5 + P1 - P3 - P7
> >    
> >    return Combine(C11, C12, C21, C22)
> >```

## 5. Comparing Optimization Techniques

>[!example] Reordering vs Blocking
> **Question:** Why might loop reordering alone fail to reach peak performance for significantly larger matrices? Which type of locality is specifically improved by blocking?
> 
> **Solution:**
> - **Limits of Reordering:** Loop reordering optimizes **spatial locality** by ensuring sequential memory access, but it does not address **capacity misses**. For massive matrices, even a single row might exceed the cache size. By the time the algorithm finishes processing a row and needs to reuse elements, those elements have already been evicted from the cache.
> - **Blocking Improvement:** Blocking specifically improves **temporal locality**. By operating on small sub-matrices (blocks) that fit entirely within the cache, the algorithm maximizes the reuse of each loaded element before it is evicted.