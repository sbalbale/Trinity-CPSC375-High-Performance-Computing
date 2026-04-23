---
aliases: [OpenMP Reference, OMP Pragmas Sheet]
tags: [#exam/code, #prep/moc, #course_hpc]
sources: [Getting Started with OpenMP Programming.txt, More on OpenMP Programming.txt, Homework-18-Solutions.md, Homework-19-Solutions.md]
created: 2026-04-20
updated: 2026-04-20
---

# Cheat Sheet: OpenMP Reference

> [!abstract]
> Key OpenMP compiler directives (pragmas) and scoping rules for shared-memory parallelization.

## 1. Directives
> [!code]
> - **Region:** `#pragma omp parallel` (Forks team)
> - **Loops:** `#pragma omp parallel for` (Distributes iterations)
> - **Sections:** `#pragma omp parallel sections` (Functional parallelism)
> - **Synchronization:**
>     - `#pragma omp critical` (One thread at a time)
>     - `#pragma omp atomic` (Optimized single update)
>     - `#pragma omp barrier` (Team wait point)
>     - `#pragma omp single` / `master` (One thread only)

## 2. Data Sharing Clauses
| Clause | Scope | Initial Value | Final Value |
| :--- | :--- | :--- | :--- |
| `shared(x)` | Global | From Master | Persists |
| `private(x)` | Local | **Trash** | Lost |
| `firstprivate(x)` | Local | From Master | Lost |
| `lastprivate(x)` | Local | **Trash** | Last iter to Master |

## 3. Reduction
> [!code]
> `#pragma omp parallel for reduction(+:result)`
> - **Effect:** Creates private copies, initializes to identity (0 for `+`), and combines at the join point.

## 4. Scheduling
> [!code]
> `#pragma omp parallel for schedule(type, chunk)`
> - **static:** Fixed pre-assignment. Low overhead.
> - **dynamic:** Runtime assignment. Good for **load balancing** irregular work.

## 5. Timing
> [!code]
> ```c
> double start = omp_get_wtime();
> // Work...
> double end = omp_get_wtime();
> ```

## 6. Pitfalls
> [!warning]
> - **Race Condition:** Concurrent write to `shared` without `critical`/`atomic`/`reduction`.
> - **Deadlock:** Nested `critical` sections with different names.
> - **False Sharing:** Multiple threads writing to different variables in the same cache line.
