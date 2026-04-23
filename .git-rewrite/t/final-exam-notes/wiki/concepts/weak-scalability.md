---
aliases: [Scaled Scaling]
tags: [performance, scalability]
sources: [lec27.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Weak Scalability

> [!abstract]
> **Weak Scalability** measures the ability of a parallel system to maintain its efficiency as the **problem size increases proportionally** with the number of processors. It is used to evaluate how well a system handles growing workloads.

## Core Mechanics

- **Workload**: Scaled (e.g., $10^6$ elements per processor).
- **Goal**: Maintain constant execution time ($T_p \approx \text{const}$).
- **Metric**: Efficiency ($E \approx 1$).

> [!warning] Scaling Bottlenecks
> Even in weak scaling, communication costs often grow with the number of processors (e.g., $O(\log p)$ for broadcasts). Therefore, perfect weak scaling ($E=1$) is rare for large-scale systems.

## Implementations & Examples

> [!example] Weak Scaling Example
> Simulating weather at different resolutions.
> - On 1 node: $100 \times 100$ grid in 10 minutes.
> - On 4 nodes: $200 \times 200$ grid (4x more work) in 11 minutes.
> - On 16 nodes: $400 \times 400$ grid (16x more work) in 12 minutes.
> The time remains relatively stable as the problem size scales with the hardware.

## Connections
* **Prerequisites:** [[parallel-efficiency]], [[gustafsons-law]].
* **Used In:** Large-scale scientific simulations.
* **Contrasts With:** [[strong-scalability]].
