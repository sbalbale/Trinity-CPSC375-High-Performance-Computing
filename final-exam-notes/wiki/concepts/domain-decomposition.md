---
aliases: [Data Decomposition, Domain Decomposition]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf, HW15_Solution.md]
created: 2026-04-20
updated: 2026-04-21
---

# Domain Decomposition

> [!abstract] TL;DR Summary
> A [[Partitioning]] strategy where the **data** (e.g., an array, matrix, or spatial grid) is divided into pieces, and each task performs the same operation on its piece of data.

## Core Mechanics
- **Focus:** Data structures (input data, output data, or intermediate results).
- **Process:** Divide the primary data structure into sub-domains and associate computation with each piece.

> [!example] 3D Weather Forecasting Model
> - **Data Division**: Partition the Earth's 3D atmosphere grid (longitude, latitude, altitude) into **3D sub-volumes** or vertical "columns."
> - **Primitive Task**: Represents the computation required to advance a **single grid cell** (one spatial point) forward by one time step (e.g., applying fluid dynamics PDEs).
> - **Communication**: Natural candidate for local communication. Each task requires data from its 6 immediate neighbors (up, down, N, S, E, W). Boundary tasks exchange "halo" values.

> [!warning] Common Pitfalls
> - **Boundary Conditions**: Communication is often required to exchange data at the boundaries of decomposed domains.
> - **Surface-to-Volume Ratio**: Agglomerating in 3D (cubes) is more efficient than 1D (slices) as it minimizes the data to be communicated relative to the computational work.

## Connections
* **Type Of:** [[Partitioning]]
* **Contrasts With:** [[Functional Decomposition]]
