---
aliases: [Data Race, Non-determinism, Race Condition]
tags: 
  - #exam/theory
  - #CPSC375
sources: [Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Race Condition

> [!abstract]
> A **Race Condition** occurs when the output of a program depends on the unpredictable timing or interleaving of multiple threads. In parallel computing, this typically happens when multiple threads access and modify **shared data** without proper synchronization.

## Core Mechanics

### The "Read-Modify-Write" Conflict
Consider the operation `sum += 1`:
1. **Thread A** reads `sum` (value 10).
2. **Thread B** reads `sum` (value 10).
3. **Thread A** increments and writes 11.
4. **Thread B** increments and writes 11.
*The result is 11, but it should have been 12.* One update was "lost."

> [!warning] Common Pitfalls
> - **Heisenbugs:** Race conditions often disappear when debugging (due to timing changes), making them extremely difficult to isolate.
> - **Default Sharing:** In OpenMP, global variables are **shared** by default, making them prime candidates for race conditions.

## Implementations & Examples

### Fixing Race Conditions in OpenMP
1. **Critical Sections**: Use `#pragma omp critical` to ensure only one thread executes the update at a time.
2. **Reductions**: Use the `reduction(+:sum)` clause to give each thread a private copy and combine them safely at the end.

## Connections
* **Prerequisites:** [[shared-memory-model]].
* **Used In:** [[acid-properties]] (Isolation).
* **Contrasts With:** [[reduction-operation]], [[omp-synchronization]].
