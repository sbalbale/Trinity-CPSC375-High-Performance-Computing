---
aliases: [Task Grouping, Agglomeration]
tags: 
  - #exam/theory
  - #hpc
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Agglomeration

> [!abstract] TL;DR Summary
> The third stage of [[Foster's Methodology]] where fine-grained tasks are grouped into **larger tasks** to improve performance. This reduces **communication costs** and **management overhead** on real-world hardware.

## Goals of Agglomeration
1. **Reduce Communication overhead**: Eliminate inter-task communication within the group and combine multiple small messages into fewer, larger messages.
2. **Maintain Scalability**: Ensure the number of groups is still significantly larger than the processor count to allow for future scaling.
3. **Reduce Engineering Cost**: Simplifying the task graph makes the final program easier to implement and debug.

## Agglomeration Strategies
- **Communication-Based**: Group tasks that communicate frequently to eliminate network traffic.
- **Load-Based**: Group tasks to ensure even work distribution across processors.
- **Memory-Based**: Group tasks that share data structures to improve **cache locality**.

## Core Mechanics
- **Objective:** Balance the number of tasks with the number of processors.
- **Benefits:**
  - Combines multiple communication steps into one.
  - Reduces task creation/scheduling overhead.

> [!warning] Common Pitfalls
> - **Reduced Parallelism:** Agglomerating too much can leave processors idle if the number of tasks becomes less than the number of processors.
> - **Increased Memory Pressure:** Larger tasks might require more memory per processor.

## Connections
* **Prerequisites:** [[Communication]]
* **Used In:** [[Mapping (Parallel Programming)]]
