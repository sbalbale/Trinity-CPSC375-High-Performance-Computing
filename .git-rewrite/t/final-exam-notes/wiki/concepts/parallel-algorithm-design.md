---
aliases: [PCAM, Designing Parallel Algorithms]
tags: [theory, parallel-programming, design]
sources: [lec15.pdf, lec16.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Algorithm Design

> [!abstract]
> **Parallel Algorithm Design** is the process of transforming a sequential problem into an efficient parallel implementation. The industry-standard approach is **Foster's Methodology**, which uses the **PCAM** (Partitioning, Communication, Agglomeration, Mapping) framework.

## Core Mechanics

The design process follows four distinct stages:
1. **Partitioning**: Breaking the problem and data into small tasks.
2. **Communication**: Specifying how tasks interact and exchange data.
3. **Agglomeration**: Combining tasks to improve performance and simplify implementation.
4. **Mapping**: Assigning tasks to physical processors.

## Goals
- **Minimize Overhead**: Reduce communication and synchronization costs.
- **Maximize Speedup**: Ensure the algorithm scales as processors are added.
- **Maintain Load Balance**: Keep all processors busy with useful work.

## Connections
* **Prerequisites:** Sequential algorithm understanding.
* **Used In:** All parallel programming projects.
* **Consists Of:** [[fosters-methodology]].
