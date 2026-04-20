---
aliases: [Exclusive Scan, Prefix Sum]
tags: [mpi, collective]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI_Exscan

> [!abstract]
> **MPI_Exscan** is a collective communication routine that performs an **exclusive scan** operation across all processes in a communicator. It is primarily used to calculate global offsets for distributed data structures.

## Core Mechanics

> [!equation] Exclusive Scan Rule
> Given values $\{v_0, v_1, \dots, v_{p-1}\}$ on processes $\{P_0, P_1, \dots, P_{p-1}\}$:
> - $P_0$ receives nothing (or identity).
> - $P_i$ receives $\sum_{j=0}^{i-1} v_j$ (the sum of all values on processors with a **lower rank**).

### Differences from MPI_Scan
- **Inclusive Scan (MPI_Scan)**: $P_i$ receives $\sum_{j=0}^{i} v_j$ (includes its own value).
- **Exclusive Scan (MPI_Exscan)**: $P_i$ receives $\sum_{j=0}^{i-1} v_j$ (excludes its own value).

> [!warning] Undefined for P0
> The receive buffer on $P_0$ is undefined after an `MPI_Exscan` call. Applications must handle this (e.g., by initializing the buffer to 0).

## Implementations & Examples

> [!example] Global Offset Calculation
> In a parallel Radix sort, if each process $P_i$ has $c_i$ elements in a bucket, `MPI_Exscan` allows $P_i$ to find its starting index in the global sorted array by summing the counts of all previous processes.

## Connections
* **Prerequisites:** [[mpi]], [[reduction-operation]].
* **Used In:** [[radix-sort]], [[data-partitioning]].
* **Contrasts With:** `MPI_Scan`, `MPI_Reduce`.
