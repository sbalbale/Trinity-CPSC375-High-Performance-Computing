---
aliases: [Fixed Mapping, Compile-time Mapping]
tags: [#exam/theory, #hpc]
sources: [lec16.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Static Mapping

> [!abstract] TL;DR Summary
> Assignment of tasks to processors **before execution**. Ideal when the number of tasks is fixed and computational costs are **predictable**.

## Core Mechanics
* **Block Distribution:** Contiguous chunks of data are assigned to processors. Good for spatial locality.
* **Cyclic Distribution:** Tasks assigned round-robin. Helps balance load if computational cost varies linearly or predictably across the dataset.

> [!warning] Common Pitfalls
> * **Load Imbalance:** If some tasks take significantly longer than others and this isn't accounted for in the static assignment, processors will sit idle.
> * **Inflexibility:** Cannot adapt to runtime changes in resource availability or data-dependent execution times.

## Connections
* **Antonym:** [[Dynamic Mapping]]
* **Used In:** [[Matrix Multiplication]], [[Domain Decomposition]]
