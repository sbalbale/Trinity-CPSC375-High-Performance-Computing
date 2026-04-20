---
aliases: [Data Race]
tags: 
  - #exam/theory
  - #CPSC375
sources: [Getting Started with OpenMP Programming.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Race Condition

> [!abstract] TL;DR Summary
> A **Race Condition** occurs when multiple threads concurrently access and modify a shared variable without proper synchronization, leading to **unpredictable and non-deterministic results**.

## Core Mechanics

> [!warning] Common Pitfalls
> - **Silent Failures:** Race conditions do not cause compilation errors and might not manifest on every run, making them notoriously hard to debug.

## Implementations & Examples

> [!example] Worked Problems
> If two threads simultaneously execute `sum += 1` on a shared `sum`, they might both read the same initial value and write back the same incremented value, effectively losing one increment operation.

## Connections
* **Prerequisites:** [[Parallel Region]]
* **Contrasts With:** Proper synchronization using `critical` sections or [[Reduction Operation]].
