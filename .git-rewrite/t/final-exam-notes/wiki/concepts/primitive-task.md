---
aliases: [Atomic Task, Unit of Work]
tags: [theory, parallel-programming, design]
sources: [lec15.pdf, Homework 15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Primitive Task

> [!abstract]
> A **Primitive Task** is the smallest indivisible unit of work identified during the [[partitioning]] phase of parallel algorithm design. It typically involves a set of computations on a single element or a small subset of the data.

## Role in PCAM
1. **Partitioning**: The designer breaks the problem into thousands or millions of primitive tasks to expose maximum parallelism.
2. **Agglomeration**: These tasks are later grouped into larger composite tasks to reduce communication overhead and adapt to the specific number of physical processors.

> [!example] Scientific Simulation
> In a weather forecasting model on a 3D grid, a primitive task might represent the computation of atmospheric variables for a **single grid cell**.

## Connections
* **Prerequisites:** [[fosters-methodology]].
* **Used In:** [[mapping-parallel-programming]], [[load-balancing]].
* **Contrasts With:** Composite task (an agglomerated group of primitive tasks).
