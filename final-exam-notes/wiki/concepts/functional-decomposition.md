---
aliases: [Task Decomposition, Pipeline Parallelism]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Functional Decomposition

> [!abstract] TL;DR Summary
> A [[Partitioning]] strategy where the **computation** itself is divided into distinct tasks. Each task represents a different part of the overall algorithm.

## Core Mechanics
- **Focus:** The flow of execution/functions.
- **Example:** In a video player, one task might decode frames, another handles audio, and a third handles rendering.

> [!warning] Common Pitfalls
> - **Pipeline Stalls:** If one function is significantly slower than others, it becomes a bottleneck for the entire functional pipeline.

## Connections
* **Type Of:** [[Partitioning]]
* **Contrasts With:** [[Domain Decomposition]]
