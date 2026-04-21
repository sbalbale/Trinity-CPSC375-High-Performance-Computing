---
aliases: [Algorithmic Efficiency, Resource Efficiency]
tags: [performance, theory, metrics]
sources: [lec25.txt, Homework 25.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Work Efficiency

> [!abstract]
> **Work Efficiency** is a property of a parallel algorithm where the total work performed (across all processors) is asymptotically equal to the complexity of the best sequential algorithm. $W_p \in O(W_s)$.

## Core Mechanics

- **Work ($W_p$)**: The sum of all operations performed by all processors during parallel execution.
- **Why it matters**: An algorithm that is not work-efficient might achieve fast parallel time but wastes massive amounts of computational resources, making it impractical for very large scale.

> [!example] Mergesort
> Sequential mergesort is $O(n \log n)$. Parallel mergesort also performs $O(n \log n)$ total work, making it **work-efficient**.

## Connections
* **Prerequisites:** [[parallel-efficiency]].
* **Used In:** Comparing algorithms (e.g., bitonic sort vs mergesort).
* **Contrasts With:** Parallel speedup (which focuses on time, not total work).
