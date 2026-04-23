---
aliases: [Scalable Universal Matrix Multiplication Algorithm]
tags: [algorithm, matrix, distributed, scalable]
sources: [lec31.txt, Homework 31.pdf]
created: 2026-04-20
updated: 2026-04-23
---

# SUMMA Algorithm

> [!abstract]
> **SUMMA** (Scalable Universal Matrix Multiplication Algorithm) is a highly versatile distributed algorithm that computes matrix products as a sum of **outer products**. It is the standard algorithm used in modern libraries like ScaLAPACK.

## Core Mechanics

> [!abstract] Flexibility
> Unlike Cannon's or Fox's algorithms, SUMMA's panel broadcast approach allows processors to be arranged in an arbitrary $p_r \times p_c$ grid. It removes the restriction that $p$ must be a perfect square.

### Rank-b Updates
Instead of individual elements or large blocks, SUMMA uses **panels** (stripes of width $b$):
1. **Divide**: Matrix A and B are divided into panels of width $b$.
2. **Broadcast**: 
    - The current panel of A is broadcast along its processor row.
    - The current panel of B is broadcast along its processor column.
3. **Compute**: Every processor performs a **rank-b update** on its local portion of matrix C ($C_{local} += A_{panel} \times B_{panel}$).

### Pipelining
SUMMA can **hide communication latency** by pipelining:
- A processor can start the rank-$b$ update for iteration $k$ while the broadcasts for iteration $k+1$ are still in transit.

> [!warning] Parameter Tuning
> The performance of SUMMA is sensitive to the **panel width $b$**. A larger $b$ increases computational throughput but also increases communication latency per step.

## Connections
* **Prerequisites:** [[checkerboard-decomposition]], [[collective-communication]].
* **Used In:** ScaLAPACK, high-performance linear algebra libraries.
* **Contrasts With:** [[foxs-algorithm]] (SUMMA is generally more scalable).
