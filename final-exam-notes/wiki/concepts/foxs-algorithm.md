---
aliases: [Broadcast-and-Shift Matrix Mult]
tags: [algorithm, matrix, distributed, mpi]
sources: [lec31.txt, Homework 31.pdf]
created: 2026-04-20
updated: 2026-04-23
---

# Fox's Algorithm

> [!abstract]
> **Fox's Algorithm** is a distributed matrix multiplication algorithm that uses a **row-broadcast and column-shift** pattern. It is often more intuitive than Cannon's because it mimics the outer-product definition of matrix multiplication.

## Core Mechanics

### The Algorithm
Iteration $k$ (from 0 to $q-1$, where $q = \sqrt{p}$):
1. **Broadcast A**: In each row $i$, the process at column **$(i+k) \pmod{q}$** broadcasts its current block of matrix A to all other processes in its row.
2. **Multiply**: Each process $P_{ij}$ multiplies the received A-block with its local B-block: $C_{ij} += A_{recvd} \times B_{local}$.
3. **Shift B**: Each process $P_{ij}$ shifts its local B-block vertically upwards to $P_{i-1,j}$ and receives a new B-block from $P_{i+1,j}$ (using a circular shift).

> [!warning] Fox vs. Cannon
> - **Cannon's**: Initial skewing is complex but operates **in-place** (no extra memory).
> - **Fox's**: Broadcast logic is simpler and more intuitive but requires an **extra buffer** to store the broadcasted block of A.

## Connections
* **Prerequisites:** [[checkerboard-decomposition]], [[collective-communication]] (Broadcast).
* **Used In:** Parallel linear algebra implementations.
* **Contrasts With:** [[cannons-algorithm]].
