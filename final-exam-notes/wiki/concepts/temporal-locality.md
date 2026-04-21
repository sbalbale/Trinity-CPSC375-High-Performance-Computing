---
aliases: [Temporal Cache Locality, Data Reuse]
tags: [performance, cache, architecture]
sources: [Homework 30.pdf]
created: 2026-04-21
updated: 2026-04-21
---

# Temporal Locality

> [!abstract]
> **Temporal Locality** is a principle of memory access where a program references the same memory location multiple times within a short duration. Maximizing temporal locality allows data to be reused while it is still in the fast, small CPU cache, preventing expensive redundant fetches from main memory.

## Core Mechanics

- **Data Reuse:** When a block of data is loaded into the cache, algorithms optimized for temporal locality will perform as many operations on that data as possible before moving on to new data.
- **Cache Eviction Avoidance:** By focusing work on a small "working set" that fits in the cache, temporal locality prevents the cache from thrashing (where useful data is constantly evicted to make room for new data, only to be needed again shortly after).

> [!example] Blocking (Tiling)
> Standard matrix multiplication loads entire rows and columns. For large matrices, the first elements of a row are evicted before they can be reused for the next column. **Blocking (Tiling)** divides the matrix into smaller sub-matrices that fit entirely within the cache, dramatically improving temporal locality by reusing the sub-matrix elements multiple times.

## Connections
* **Prerequisites:** Cache architecture.
* **Used In:** [[matrix-multiplication-optimization]], [[tiling-block-matrix]].
* **Contrasts With:** [[spatial-locality]] (accessing nearby addresses).