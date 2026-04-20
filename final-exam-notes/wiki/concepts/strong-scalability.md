---
aliases: [Fixed-Size Scaling]
tags: [performance, scalability]
sources: [lec27.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Strong Scalability

> [!abstract]
> **Strong Scalability** measures the ability of a parallel system to reduce the total execution time for a **fixed problem size** by adding more processors. It is fundamentally limited by the serial portion of the workload.

## Core Mechanics

- **Workload**: Fixed.
- **Goal**: Minimize latency ($T_p$).
- **Metric**: Speedup ($S_p$) and Efficiency ($E$).

> [!warning] The Amdahl Limit
> As more processors are added to a fixed problem, the amount of work per processor decreases. Eventually, the **parallel overhead** and the **serial fraction** dominate the execution time, causing efficiency to drop to zero.

## Implementations & Examples

> [!example] Strong Scaling Example
> Sorting 1 million integers.
> - On 1 node: 10 seconds.
> - On 2 nodes: 5.5 seconds.
> - On 4 nodes: 3.2 seconds.
> As we add nodes, we expect the time to drop, but the speedup will likely be sub-linear due to communication overhead.

## Connections
* **Prerequisites:** [[parallel-speedup]], [[amdahls-law]].
* **Used In:** Real-time processing, interactive applications.
* **Contrasts With:** [[weak-scalability]].
