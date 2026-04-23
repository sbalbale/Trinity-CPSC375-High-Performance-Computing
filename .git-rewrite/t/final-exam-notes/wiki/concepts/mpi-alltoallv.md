---
aliases: [Variable All-to-All, Data Redistribution]
tags: [mpi, collective]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI_Alltoallv

> [!abstract]
> **MPI_Alltoallv** is a collective communication routine that allows every process in a communicator to send and receive **varying amounts of data** from every other process. It is the most general form of all-to-all communication.

## Core Mechanics

### The V-Variant
Unlike `MPI_Alltoall` (which requires all messages to have the same size), `MPI_Alltoallv` uses **displacement and count arrays** to specify how much data goes to and comes from each process.

> [!equation] Necessary Arguments
> - `sendcounts[]`: How many elements to send to each rank.
> - `sdispls[]`: Starting index of each send message.
> - `recvcounts[]`: How many elements to receive from each rank.
> - `rdispls[]`: Starting index for storing each incoming message.

> [!warning] Scaling Hazard
> `MPI_Alltoallv` involves $p^2$ potential messages. On large-scale systems, this can lead to massive network congestion (the "all-to-all" bottleneck).

## Implementations & Examples

> [!example] Parallel Sorting
> In a parallel sort (e.g., Radix Sort), after each process has partitioned its data into buckets for each destination node, `MPI_Alltoallv` is used to **redistribute the data** to those nodes.

## Connections
* **Prerequisites:** [[mpi]], [[collective-communication]].
* **Used In:** [[radix-sort]], [[mapping-parallel-programming]].
* **Contrasts With:** `MPI_Alltoall` (fixed size), `MPI_Bcast` (one to all).
