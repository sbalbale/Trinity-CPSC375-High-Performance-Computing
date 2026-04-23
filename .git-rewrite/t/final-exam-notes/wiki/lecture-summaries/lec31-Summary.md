---
aliases: [Lecture 31 Summary, Matrix Multiplication II Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec31.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 31: Parallel Matrix Multiplication

> [!abstract]
> This lecture explores distributed parallel algorithms for matrix multiplication. It transitions from 1D Block-Striped partitioning to 2D Checkerboard strategies, detailing Cannon's, Fox's, and the SUMMA algorithms.

## Distributed Partitioning

### 1D Block-Striped
- **Method**: Matrices are divided into $p$ horizontal stripes of size $(n/p) \times n$.
- **Communication**: Processes form a logical ring. Matrix B stripes are shifted around the ring until every process has seen every row of B.
- **Complexity**: $O(n^3/p)$ work, $O(n^2)$ communication.
- **Ratio**: $n/p$ (Poor scalability).

### 2D Checkerboard
- **Method**: Matrix is divided into $\sqrt{p} \times \sqrt{p}$ square blocks.
- **Ratio**: $n/\sqrt{p}$ (Better scalability).

---

## 2D Parallel Algorithms

### Cannon's Algorithm
1. **Initial Alignment (Skewing)**:
    - Shift row $i$ of A left by $i$ positions.
    - Shift column $j$ of B up by $j$ positions.
2. **Shift-Multiply Phase**: Perform $\sqrt{p}$ iterations of:
    - Local block multiply: $C_{ij} = C_{ij} + (A_{local} \times B_{local})$.
    - Shift A left by 1 and B up by 1.

### Fox's Algorithm
Avoids the alignment phase of Cannon's by using a **Broadcast-and-Shift** pattern:
1. One processor in each row broadcasts its A-block to the entire row.
2. Multiply received A-block with local B-block.
3. Shift B-blocks upwards circularly.
4. Repeat $\sqrt{p}$ times.

### SUMMA (Scalable Universal Matrix Multiplication Algorithm)
Based on the **outer-product** definition of matrix multiplication.
- **Panels**: Matrices are treated as a collection of $b$-width panels.
- **Workflow**:
    - Broadcast A-panel along rows.
    - Broadcast B-panel along columns.
    - Perform a rank-$b$ update on the local C-block.
- **Efficiency**: Supports **pipelining**, allowing processes to start computation while the next panel is being broadcast.
