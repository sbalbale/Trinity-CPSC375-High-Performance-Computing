---
aliases: [omp reduction, collective reduction, MPI_Reduce, Reduction Operation]
tags: 
  - #exam/technique
  - #CPSC375
sources: [Getting Started with OpenMP Programming.txt, lec21.txt, lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Reduction Operation

> [!abstract]
> A **Reduction Operation** combines multiple values across threads or processes into a single result using a binary operator (e.g., sum, product, max, min). It is a fundamental pattern for gathering partial results in parallel computing.

## MPI Reduction Operators
| Operator | Meaning | Example |
| :--- | :--- | :--- |
| `MPI_SUM` | Addition | $10 + 20 + 30 = 60$ |
| `MPI_PROD` | Multiplication | $2 \times 3 \times 4 = 24$ |
| `MPI_MAX` | Maximum | $\max(10, 20, 30) = 30$ |
| `MPI_MIN` | Minimum | $\min(10, 20, 30) = 10$ |
| `MPI_LAND` | Logical AND | `true AND false = false` |
| `MPI_BAND` | Bitwise AND | `5 & 3 = 1` |
| `MPI_LOR` | Logical OR | `true OR false = true` |
| `MPI_BOR` | Bitwise OR | `5 | 3 = 7` |
| `MPI_MAXLOC` | Max and Location | Returns max value and its rank index. |
| `MPI_MINLOC` | Min and Location | Returns min value and its rank index. |

## Properties
1. **Associativity Required**: Every reduction operator must be associative: $(a \oplus b) \oplus c = a \oplus (b \oplus c)$. This allows for tree-based execution.
2. **Commutativity Not Required**: While many operators are commutative ($a \oplus b = b \oplus a$), MPI only strictly requires associativity. However, results for non-commutative operations may vary depending on the implementation tree.

## Core Mechanics

### OpenMP Reduction
In OpenMP, the `reduction(op:var)` clause handles private variable creation and final aggregation automatically.
> [!code] OpenMP Example
> ```c
> int total = 0;
> #pragma omp parallel for reduction(+:total)
> for (int i = 0; i < n; i++) total += array[i];
> ```

### MPI Reduction
In distributed memory, `MPI_Reduce` or `MPI_Allreduce` is used to combine values across processes.
> [!code] MPI Example
> ```c
> MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
> ```

> [!warning] Common Pitfalls
> - **Efficiency:** In OpenMP, `#pragma omp critical` can also fix race conditions for summations, but it is significantly slower than a reduction because it serializes the updates.
> - **Initialization:** In OpenMP, the private copies are automatically initialized to the identity of the operator (e.g., 0 for `+`, 1 for `*`).

## Connections
* **Prerequisites:** [[openmp]], [[mpi]].
* **Used In:** [[parallel-speedup]] calculation, [[sieve-of-eratosthenes]].
* **Contrasts With:** [[race-condition]] (which reduction prevents).
