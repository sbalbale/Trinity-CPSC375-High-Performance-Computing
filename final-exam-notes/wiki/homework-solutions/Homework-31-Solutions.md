---
aliases: [HW31, Homework 31 Solutions]
tags: [#homework/solutions, #CPSC375, #exam/algorithm, #exam/theory]
sources: [Homework 31.pdf, HW31_Solution.md]
created: 2026-04-23
updated: 2026-04-23
---

# Homework 31 Solutions

**Source:** Homework 31.pdf / HW31_Solution.md
**Date ingested:** 2026-04-23
**Type:** homework

## Summary

This homework focuses on parallel matrix multiplication algorithms, specifically comparing the scalability and communication overhead of **Block-Striped** decomposition, **Cannon's Algorithm**, **Fox's Algorithm**, and **SUMMA**.

## Problem 1: Computation-to-Communication Ratio

**Question:** Derive why the block-striped computation-to-communication ratio of $n/p$ is considered poor compared to Cannon's ratio of $n/\sqrt{p}$.

>[!example] Solution: Computation-to-Communication Ratio
> **Block-Striped Decomposition:**
> - **Computation:** $T_{\text{comp}} = O(n^3/p)$.
> - **Communication:** Each process must eventually see all $p$ stripes of $B$. Total communication volume is $(p-1) \cdot n^2/p \approx O(n^2)$.
> - **Ratio:** $\frac{T_{\text{comp}}}{T_{\text{comm}}} = \frac{n^3/p}{n^2} = \frac{n}{p}$.
> 
> **Cannon's Algorithm (Checkerboard Decomposition):**
> - **Computation:** $T_{\text{comp}} = O(n^3/p)$.
> - **Communication:** $\sqrt{p}$ iterations of two blocks of size $(n/\sqrt{p})^2 = n^2/p$. Total $T_{\text{comm}} = \sqrt{p} \cdot n^2/p = O(n^2/\sqrt{p})$.
> - **Ratio:** $\frac{T_{\text{comp}}}{T_{\text{comm}}} = \frac{n^3/p}{n^2/\sqrt{p}} = \frac{n}{\sqrt{p}}$.
> 
> **Comparison for Scalability:**
> To maintain a target ratio $R$ (where computation hides communication):
> - **Block-Striped:** $n \ge R \cdot p$ (Linear scaling).
> - **Cannon's:** $n \ge R \cdot \sqrt{p}$ ($\sqrt{p}$ scaling).
> 
> **Conclusion:** As $p$ increases, the block-striped ratio collapses $\sqrt{p}$ times faster than Cannon's. For $p=10,000$ and $n=10,000$, block-striped has a ratio of 1 (terrible), whereas Cannon's has 100 (healthy).

## Problem 2: Cannon's vs. Fox's Algorithm

**Question A:** Trace the specific movement of blocks $A_{1,2}$ and $B_{1,2}$ in a $4 \times 4$ processor mesh during initial alignment in Cannon's.

>[!example] Solution 2A: Initial Alignment Trace
> In a $4 \times 4$ mesh ($\sqrt{p} = 4$), row/col indices are $0, 1, 2, 3$.
> - **Block $A_{1,2}$:** Shifted left by $i=1$ position. New location is column $(2-1) \pmod 4 = 1$. **$P_{1,2} \to P_{1,1}$**.
> - **Block $B_{1,2}$:** Shifted up by $j=2$ positions. New location is row $(1-2) \pmod 4 = 3$. **$P_{1,2} \to P_{3,2}$**.

**Question B:** Compare the memory overhead of Fox's algorithm against Cannon's algorithm.

>[!example] Solution 2B: Memory Overhead Comparison
> - **Cannon's Algorithm:** Uses strictly **in-place** shifts. Total memory per process is $3n^2/p$ (one block each for A, B, and C).
> - **Fox's Algorithm:** Requires an **extra communication buffer** to store the broadcasted A block while keeping the original A block for later steps. Total memory is $4n^2/p$.
> - **Impact:** Fox's represents a **~33% memory increase** over Cannon's, which can be critical in memory-bound regimes (e.g., GPU HBM).

**Question C:** Identify a hardware scenario where `MPI_Bcast` optimization might make Fox's algorithm faster.

>[!example] Solution 2C: Hardware Optimization for Fox's Algorithm
> Fox's algorithm can win when the row-broadcast completes in $O(\log \sqrt{p})$ time, beating Cannon's sequential $O(\sqrt{p})$ shifts:
> 1. **Hardware Multicast:** InfiniBand or Cray networks with switch-level broadcast support.
> 2. **GPU Clusters:** NCCL/RCCL exploiting NVLink/NVSwitch topologies with tree algorithms.
> 3. **NUMA Systems:** Shared-memory broadcasts that collapse to memcpy/cache-line reads rather than discrete message exchanges.

## Problem 3: SUMMA (Scalable Universal Matrix Multiplication Algorithm)

**Question A:** Explain how rewriting the matrix product into a sum of outer products allows SUMMA to be more flexible.

>[!example] Solution 3A: Outer Product Flexibility
> Unlike Cannon's or Fox's which couple the algorithm to a square $\sqrt{p} \times \sqrt{p}$ grid, SUMMA's outer-product formulation ($C = \sum A_{:,k} \times B_{k,:}$) only requires row and column broadcasts.
> - **Arbitrary Grids:** Works on any $p_r \times p_c$ grid (e.g., $3 \times 4$ for 12 nodes).
> - **Non-square Matrices:** Naturally handles $m \times n$ products where $k$ is decoupled from the output dimensions.

**Question B:** Explain how the block size $b$ balances communication latency and pipelining efficiency.

>[!example] Solution 3B: Tuning Block Size $b$
> - **Very small $b$ (e.g., 1 or 2):** Latency cost $(n/b \cdot \alpha)$ dominates. The local update degenerates to rank-1 updates (BLAS-2), losing the BLAS-3 cache-reuse efficiency.
> - **Very large $b$ (e.g., $n/\sqrt{p}$):** Pipelining suffers because there are too few stages ($n/b$ is tiny) to hide broadcasts behind computation. Panel buffers also grow large, potentially crowding out the cache.
> - **Sweet Spot:** Typically $b \in [32, 128]$, balancing BLAS-3 throughput with enough stages for effective overlap/pipelining.
