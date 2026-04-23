---
aliases: [HW31, Homework 31 Solutions]
tags: [#homework/solutions, #CPSC375, #exam/algorithm, #exam/theory]
sources: [Homework 31.pdf]
created: 2026-04-23
updated: 2026-04-23
---

# Homework 31 Solutions

**Source:** Homework 31.pdf
**Date ingested:** 2026-04-23
**Type:** homework

## Summary

This homework focuses on parallel matrix multiplication algorithms, specifically comparing the scalability and communication overhead of **Block-Striped** decomposition, **Cannon's Algorithm**, **Fox's Algorithm**, and **SUMMA**.

## Problem 1: Computation-to-Communication Ratio

**Question:** Derive why the block-striped computation-to-communication ratio of $n/p$ is considered poor compared to Cannon's ratio of $n/\sqrt{p}$.

>[!example] Solution: Computation-to-Communication Ratio
> For parallel matrix multiplication of $n \times n$ matrices:
> 
> **Block-Striped Decomposition:**
> - Each of the $p$ processes holds a block of $n \times (n/p)$ elements.
> - **Computation:** Each process computes $n^2 / p$ elements.
> - **Communication:** Each process must shift its block $p-1$ times. In each shift, it sends $n^2 / p$ elements.
> - **Total Communication Volume:** $(p-1) \times (n^2 / p) \approx n^2$.
> - **Ratio:** $\frac{\text{Computation}}{\text{Communication}} = \frac{O(n^3/p)}{O(n^2)} = O(n/p)$.
> 
> **Cannon's Algorithm (Checkerboard Decomposition):**
> - Processors are arranged in a $\sqrt{p} \times \sqrt{p}$ mesh.
> - Each process holds a block of size $(n/\sqrt{p}) \times (n/\sqrt{p})$.
> - **Computation:** Each process computes $(n/\sqrt{p}) \times (n/\sqrt{p}) \times n = n^3 / p$ operations.
> - **Communication:** Each process shifts its block $\sqrt{p}-1$ times. In each shift, it sends $n^2 / p$ elements.
> - **Total Communication Volume:** $(\sqrt{p}-1) \times (n^2 / p) \approx n^2 / \sqrt{p}$.
> - **Ratio:** $\frac{\text{Computation}}{\text{Communication}} = \frac{O(n^3/p)}{O(n^2/\sqrt{p})} = O(n/\sqrt{p})$.
> 
> **Conclusion:** Cannon's ratio of $n/\sqrt{p}$ is better (larger) than the block-striped ratio of $n/p$ because $\sqrt{p} < p$. This means Cannon's algorithm spends proportionally less time communicating as the number of processors $p$ scales, making it highly scalable.

## Problem 2: Cannon's vs. Fox's Algorithm

**Question A:** Trace the specific movement of blocks $A_{1,2}$ and $B_{1,2}$ in a $4 \times 4$ processor mesh during initial alignment in Cannon's.

>[!example] Solution 2A: Initial Alignment Trace
> In a $4 \times 4$ mesh ($\sqrt{p} = 4$), processors are indexed $(i, j)$ from $0$ to $3$.
> - **$A_{i,j}$ Initial Shift:** Cycled left by $i$ positions.
> - **$B_{i,j}$ Initial Shift:** Cycled up by $j$ positions.
> 
> For block $A_{1,2}$ (located at row $i=1$, column $j=2$):
> - It is shifted left by $i=1$ position.
> - New location: Row $1$, Column $(2 - 1) \pmod 4 = 1$. It moves to $P_{1,1}$.
> 
> For block $B_{1,2}$ (located at row $i=1$, column $j=2$):
> - It is shifted up by $j=2$ positions.
> - New location: Row $(1 - 2) \pmod 4 = 3$, Column $2$. It moves to $P_{3,2}$.

**Question B:** Compare the memory overhead of Fox's algorithm against Cannon's algorithm.

>[!example] Solution 2B: Memory Overhead Comparison
> - **Cannon's Algorithm:** Uses strictly **in-place** shifts. It only requires memory to hold one block of $A$, one block of $B$, and the computed block of $C$. The memory overhead is minimal since blocks are swapped cyclically.
> - **Fox's Algorithm:** Requires an **extra communication buffer** to store the broadcasted block of $A$ along the processor rows. Because the local $A$ block must also be maintained for future steps, it inherently requires more memory per processor than Cannon's algorithm.

**Question C:** Identify a hardware scenario where `MPI_Bcast` optimization might make Fox's algorithm faster.

>[!example] Solution 2C: Hardware Optimization for Fox's Algorithm
> Fox's algorithm can outperform Cannon's on hardware with highly optimized, hardware-accelerated **multicast or broadcast networks** (e.g., specific topologies like fat-trees with switch-level broadcast support, or InfiniBand networks with hardware multicast). In these environments, `MPI_Bcast` can execute in $O(1)$ or very low latency, making the row-wise broadcast of Fox's algorithm faster than the sequential point-to-point ring shifts required by Cannon's algorithm.

## Problem 3: SUMMA (Scalable Universal Matrix Multiplication Algorithm)

**Question A:** Explain how rewriting the matrix product into a sum of outer products allows SUMMA to be more flexible.

>[!example] Solution 3A: Outer Product Flexibility
> By defining $C = \sum A_{:,k} \times B_{k,:}$, SUMMA frames matrix multiplication as a sequence of rank-k updates (outer products of a column panel of $A$ and a row panel of $B$).
> - This decoupling means the algorithm does not rely on shifting specific square blocks along a rigid 2D mesh.
> - The panel broadcast approach allows processors to be arranged in an arbitrary $p_r \times p_c$ grid (where $p_r \times p_c = p$). It completely removes the restriction that $p$ must be a perfect square, making SUMMA significantly more adaptable to available cluster sizes.

**Question B:** Explain how the block size $b$ balances communication latency and pipelining efficiency.

>[!example] Solution 3B: Tuning Block Size $b$
> In SUMMA, panels of width $b$ are broadcasted.
> - **Very small $b$:** Leads to many small messages. This increases **latency overhead** because the fixed cost of initiating a message (startup time) is incurred repeatedly, dominating the communication time.
> - **Very large $b$:** Reduces the number of messages but leads to large panels. This reduces the **efficiency of pipelining** because subsequent processors must wait longer for the entire large panel to arrive before they can begin computing. It limits the overlap between communication and computation.
> - An optimal $b$ balances these extremes, minimizing latency costs while allowing smooth overlapping of communication with computation.
