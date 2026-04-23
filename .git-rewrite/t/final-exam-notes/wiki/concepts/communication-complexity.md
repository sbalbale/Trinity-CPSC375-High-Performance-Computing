---
aliases: [Data Movement Complexity, I/O Complexity]
tags: [performance, theory, scalability]
sources: [lec30.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Communication Complexity

> [!abstract]
> **Communication Complexity** (or I/O Complexity) measures the cost of **data movement** between different levels of the memory hierarchy (e.g., RAM to Cache). In modern systems, this is often the primary factor determining an algorithm's overall runtime.

## Core Mechanics

- **Work Complexity vs. Communication**: Standard matrix multiplication has a **work complexity** of $O(n^3)$ operations. Without optimization, it also has a **communication complexity** of $O(n^3)$ memory transfers.
- **Optimization Goal**: Modern parallel algorithms aim to reduce the communication complexity even if they slightly increase the work complexity.
- **Tiling (Blocking)**: A technique that dramatically reduces communication by reusing data while it is in a faster memory level (cache).

> [!equation] Tiled Matrix Multiplication Complexity
> For a matrix of size $n$ and cache size $M$:
> - Standard Complexity: $O(n^3)$ transfers.
> - Tiled Complexity: $O\left( \frac{n^3}{\sqrt{M}} \right)$ transfers.

## Implementations & Examples

> [!example] Distributed Computing
> In MPI, communication complexity refers to the number and size of messages sent over the network (measured by latency $\lambda$ and bandwidth $\beta$).

## Connections
* **Prerequisites:** [[overhead-parallel-computing]].
* **Used In:** [[tiling-block-matrix]], distributed algorithm design.
* **Contrasts With:** Algebraic work complexity (e.g., Big-O for operations).
