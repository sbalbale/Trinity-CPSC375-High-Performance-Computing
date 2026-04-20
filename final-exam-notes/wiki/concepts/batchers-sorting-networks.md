---
aliases: [Switching Networks, Parallel Sorting Networks]
tags: [architecture, sorting, hardware]
sources: [lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Batcher's Sorting Networks

> [!abstract]
> **Batcher's Sorting Networks** are hardware-inspired parallel sorting designs that use fixed patterns of **Compare-and-Exchange** units. The two primary designs are **Odd-Even Mergesort** and **Bitonic Mergesort**, both achieving $O(\log^2 n)$ parallel time.

## Core Mechanics

- **Fixed Topology**: The "wires" (data paths) and "switches" (compare-and-exchange units) are fixed, making them highly suitable for VLSI and FPGA implementations.
- **Data-Independent**: The number of comparison steps depends only on the number of elements $n$, not on the values themselves.
- **Complexity**: $O(n \log^2 n)$ total comparisons, but $O(\log^2 n)$ parallel time with $n$ processors.

### Network Types
1. **Odd-Even Mergesort Network**: Based on merging odd and even indices.
2. **Bitonic Mergesort Network**: Based on transforming the list into a large bitonic sequence.

> [!warning] Hardware Overhead
> Implementing these networks for very large $n$ requires a massive number of comparators ($O(n \log^2 n)$), which can be physically prohibitive.

## Implementations & Examples

> [!example] Comparison
> For $n = 8$:
> - A sorting network will have $\log^2 8 = 3 \times 4 / 2 = 6$ stages of parallel comparisons.

## Connections
* **Prerequisites:** [[compare-and-exchange]], [[bitonic-mergesort]].
* **Used In:** GPUs, network switches, and custom ASIC sorting hardware.
* **Contrasts With:** $O(n \log n)$ sequential sorting (which is faster but harder to fully parallelize in a fixed network).
