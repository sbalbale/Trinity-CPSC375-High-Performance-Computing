---
aliases: [Parallel Linear Search, Distributing Search]
tags: [algorithm, search, parallel-programming]
sources: [lec16.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Search

> [!abstract]
> **Parallel Search** involves finding a specific target value in a large dataset by dividing the data among multiple processors. Each processor searches its local subset and reports back if the target is found.

## Core Mechanics

### Implementation Strategy
1. **Decomposition**: Divide the array/dataset using [[block-decomposition]].
2. **Local Search**: Each processor performs a sequential search on its assigned block.
3. **Communication**:
    - If found, the processor can signal others to stop (optional, involves global synchronization).
    - Use `MPI_Reduce` or `MPI_Gather` to collect the index or result.

> [!warning] Termination
> In some implementations, a processor might finish its search and remain idle while others continue. Implementing a "global exit" when the first processor finds the target requires careful asynchronous communication to avoid high overhead.

## Connections
* **Prerequisites:** [[partitioning]], [[block-decomposition]].
* **Used In:** Database queries, pattern matching.
* **Contrasts With:** Sequential linear search.
