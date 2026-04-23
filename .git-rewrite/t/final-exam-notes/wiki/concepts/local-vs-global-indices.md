---
aliases: [Local Indexing, Global Indexing]
tags: [parallel-programming, programming]
sources: [lec23.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Local vs. Global Indices

> [!abstract]
> In parallel programming, a **Global Index** refers to an element's position in the entire dataset, while a **Local Index** refers to its position within a process's private memory. Mapping between these is essential for correct data access and communication.

## Core Mechanics

> [!equation] Conversions (Block Decomposition)
> Given a process with `low_global_index`:
> - **Global to Local**: $i_{local} = i_{global} - low\_global\_index$
> - **Local to Global**: $i_{global} = i_{local} + low\_global\_index$

> [!warning] Common Pitfalls
> - **Out-of-Bounds Access**: Using a global index to access a local array without conversion will likely cause a segmentation fault or incorrect data modification.
> - **Logic Errors**: Loop conditions in parallel code often use `local_size`, but the computations inside the loop might need the `global_index` (e.g., to check if $i_{global} \pmod k = 0$).

## Implementations & Examples

> [!code] Looping Over Parallel Elements
> ```c
> size = BLOCK_SIZE(id, p, n);
> low = BLOCK_LOW(id, p, n);
> for (i = 0; i < size; i++) {
>     int global_i = i + low;
>     // Process global_i using local array element local_array[i]
> }
> ```

## Connections
* **Prerequisites:** [[block-decomposition]].
* **Used In:** [[sieve-of-eratosthenes]], any parallelized array operation.
* **Contrasts With:** Shared memory models (where global indices are often used directly).
