---
aliases: [Data Locality, Memory Locality]
tags: [performance, cache, architecture]
sources: [lec30.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Spatial Locality

> [!abstract]
> **Spatial Locality** is a performance property where a program accesses memory locations that are **close to each other** in space. Maximizing spatial locality is the primary goal of cache-friendly algorithm design.

## Core Mechanics

- **Cache Line Utilization**: When a CPU requests a word from memory, it loads an entire **cache line** (e.g., 64 bytes). If the program then accesses the next word in that line, it is a **cache hit**.
- **Sequential Access**: Iterating through an array sequentially ($A[i], A[i+1], \dots$) is the most efficient pattern for spatial locality.
- **Stride-n Access**: Accessing memory with a large gap (stride) between locations (e.g., column-major access in a row-major array) ruins spatial locality.

> [!warning] Stride-n Bottleneck
> In matrix multiplication, the standard `ijk` order results in a stride-$n$ access to Matrix B. This forces the CPU to load a new cache line for every single multiplication, wasting memory bandwidth and stalling the processor.

## Implementations & Examples

> [!example] Loop Reordering
> Changing the loop order from `ijk` to `ikj` ensures that the inner-most loop iterates over the row of both Matrix B and Matrix C. This changes the access pattern to **sequential**, maximizing spatial locality.

## Connections
* **Prerequisites:** Cache architecture.
* **Used In:** [[matrix-multiplication-optimization]], [[cache-optimization-parallel]].
* **Contrasts With:** Temporal locality (reusing the same address).
