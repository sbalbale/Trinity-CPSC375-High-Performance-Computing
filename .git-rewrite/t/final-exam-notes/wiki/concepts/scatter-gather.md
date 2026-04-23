---
aliases: [Data Distribution and Collection, MPI_Scatter, MPI_Gather]
tags: [mpi, collective, communication]
sources: [lec21.pdf, Homework 21.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Scatter and Gather

> [!abstract]
> **Scatter and Gather** are dual collective communication operations. **Scatter** distributes a large array from a root process into equal-sized chunks across all processes. **Gather** collects equal-sized chunks from all processes back into a single array at the root.

## Core Mechanics

### MPI_Scatter
- **Input (Root)**: A buffer containing $p \times \text{count}$ elements.
- **Output (All)**: A buffer containing `count` elements.
- **Goal**: Divide a dataset among workers for parallel processing.

### MPI_Gather
- **Input (All)**: A buffer containing `count` elements.
- **Output (Root)**: A buffer containing $p \times \text{count}$ elements.
- **Goal**: Collect partial results from workers back to the master.

> [!warning] Equal Chunks
> Standard `MPI_Scatter` and `MPI_Gather` require all chunks to be of the **same size**. For irregular distributions, use the "V" variants: `MPI_Scatterv` and `MPI_Gatherv`.

## Connections
* **Prerequisites:** [[mpi]], [[collective-communication]].
* **Used In:** [[trapezoidal-rule-mpi]], [[data-partitioning]].
* **Contrasts With:** [[reduction-operation]] (which combines elements rather than just concatenating them).
