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
