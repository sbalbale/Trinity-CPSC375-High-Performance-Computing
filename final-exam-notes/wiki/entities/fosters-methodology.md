---
aliases: [PCAM, Foster's Methodology, Foster's Design Methodology, Parallel Algorithm Design-Summary]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf, lec16.pdf]
created: 2026-04-20
updated: 2025-05-15
---

# Foster's Methodology

> [!abstract] TL;DR Summary
> A structured four-stage framework (**PCAM**) for designing parallel algorithms. It guides the designer from initial problem decomposition to final physical mapping on hardware, ensuring **scalability** and **load balance**.

## Core Mechanics

> [!warning] Common Pitfalls
> - **Premature Agglomeration:** Combining tasks too early can limit the exposure of parallelism.
> - **Ignoring Communication:** Failing to account for communication overhead during partitioning leads to poor performance on distributed systems.
> - **Static Mapping in Dynamic Loads:** Using static mapping for workloads with unpredictable execution times causes **idle processors**.

## Implementations & Examples

> [!example] The PCAM Stages
> 1. **Partitioning**: Decompose the problem into the smallest possible tasks (expose maximum parallelism).
> 2. **Communication**: Determine how tasks exchange data (local vs. global).
> 3. **Agglomeration**: Group small tasks into larger ones to reduce overhead.
> 4. **Mapping**: Assign tasks to physical processors to balance the load.

## Connections
* **Used In:** [[parallel-algorithm-design]]
* **Consists Of:** [[partitioning]], [[communication]], [[agglomeration]], [[mapping-parallel-programming]]
