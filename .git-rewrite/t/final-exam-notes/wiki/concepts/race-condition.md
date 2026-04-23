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

### The "Read-Modify-Write" Conflict (Root Cause)
A simple operation like `sum += 1;` is not **atomic**; it actually consists of three distinct low-level steps:
1. **Read**: The processor loads the current value of `sum` from memory into a register.
2. **Add**: The processor adds 1 to the value in the register.
3. **Write**: The processor stores the new value from the register back into the memory location of `sum`.

**The Race**:
When multiple threads execute these steps concurrently:
- **Thread A** reads 10.
- **Thread B** reads 10 (before A has written).
- **Thread A** increments to 11 and writes 11.
- **Thread B** increments to 11 and writes 11.
The final value is 11, even though two increments occurred. This is a **data race** because multiple threads access a shared variable without synchronization, and at least one access is a write.

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
