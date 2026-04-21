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
## Task/Channel Model
Foster's methodology is built on the **Task/Channel Model**, where a parallel computation is viewed as a set of tasks interacting through channels.
- **Task**: Includes a program, local memory, and I/O ports.
- **Channel**: A message-passing link between tasks for communication and synchronization.

## Design Goals
The methodology aims to create algorithms that are:
- **Scalable**: Performance increases with more processors.
- **Efficient**: Minimal waste of computational resources.
- **Implementable**: Straightforward to translate into code (e.g., MPI).

## Core Mechanics

> [!example] The PCAM Stages
> 1. **Partitioning**: Divide computation and data into primitive tasks.
> 2. **Communication**: Identify local and global data flow requirements.
> 3. **Agglomeration**: Group tasks to reduce communication-to-computation ratios.
> 4. **Mapping**: Assign agglomerated tasks to physical processors.
## Connections
* **Used In:** [[parallel-algorithm-design]]
* **Consists Of:** [[partitioning]], [[communication]], [[agglomeration]], [[mapping-parallel-programming]]
