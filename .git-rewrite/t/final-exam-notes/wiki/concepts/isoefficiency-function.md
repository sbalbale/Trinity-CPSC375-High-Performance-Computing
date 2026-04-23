---
aliases: [Scalability Metric, Work-Overhead Relationship]
tags: [performance, scalability, exam/formula]
sources: [lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Isoefficiency Function

> [!abstract]
> The **Isoefficiency Function** determines the rate at which the problem size (work $W$) must increase as a function of the number of processors ($p$) to maintain a **constant parallel efficiency**. It is a formal measure of an algorithm's scalability.

## Core Mechanics

> [!equation] The Isoefficiency Formula
> $$W = K \cdot T_o(p, W)$$
> - $W$: Total amount of computation (serial time $T_s$).
> - $T_o$: Total parallel overhead ($p \cdot T_p - W$).
> - $K = E / (1 - E)$: A constant derived from the desired efficiency $E$.

### Interpreting Complexity Classes
| Complexity | Scalability | Description |
| :--- | :--- | :--- |
| $O(p)$ | **Highly Scalable** | Data per processor stays constant. |
| $O(p \log p)$ | **Scalable** | Data per processor must grow slightly. |
| $O(p^2)$ | **Poorly Scalable** | Data must grow quadratically (quadruple data to double $p$). |
| $O(2^p)$ | **Non-Scalable** | Overhead grows too fast for practical scaling. |

> [!warning] Scaling Bottlenecks
> If an algorithm's isoefficiency is $O(p^2)$, running on 10x more processors requires 100x more data to keep them busy. This often exceeds the memory capacity of the system.

## Implementations & Examples

> [!example] Binary Tree Reduction
> - $W = n$ (additions)
> - Each of $p$ processes does $\log p$ communications.
> - $T_o = p \log p$
> - **Isoefficiency**: $n = K \cdot p \log p \implies O(p \log p)$.

## Connections
* **Prerequisites:** [[parallel-efficiency]], [[overhead-parallel-computing]].
* **Used In:** Comparing the scalability of different parallel algorithms (e.g., sorting).
* **Contrasts With:** [[karp-flatt-metric]] (experimental vs theoretical).
