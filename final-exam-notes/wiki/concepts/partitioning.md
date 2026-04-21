---
aliases: [Task Decomposition, Partitioning]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Partitioning

> [!abstract] TL;DR Summary
> The first stage of [[Foster's Methodology]] where a problem is decomposed into a **large number of small tasks**. The primary goal is to **expose the maximum amount of parallelism**.

## Partitioning Checklist
When designing a partition, ensure these four properties are met:
1. **Maximum Concurrency**: Identify at least an order of magnitude more tasks than processors (e.g., 160 tasks for 16 processors).
2. **Minimal Redundancy**: Avoid redundant computations and data storage across tasks.
3. **Task Balance**: Ensure primitive tasks are roughly equal in size to enable load balancing.
4. **Scalability**: The number of tasks should increase as the problem size increases.

> [!example] Image-Guided Brain Surgery
> - **Domain Decomposition**: Divide the 3D medical image into small sub-volumes (voxels), with each task analyzing one region.
> - **Functional Decomposition**: Divide the stages of the procedure (segmentation, reconstruction, visualization) into separate task groups.

## Core Mechanics
- **Goal:** Identify the finest granularity of work possible.
- **Methods:**
  - [[Domain Decomposition]]: Partitioning the **data**.
  - [[Functional Decomposition]]: Partitioning the **computation**.

> [!warning] Common Pitfalls
> - **Redundancy:** Assigning the same computation to multiple tasks unnecessarily.
> - **Scalability:** If the number of tasks does not increase with problem size, the algorithm won't scale.

## Connections
* **Prerequisites:** [[Foster's Methodology]]
* **Used In:** [[Communication]], [[Agglomeration]]
* **Types:** [[Domain Decomposition]], [[Functional Decomposition]]
