---
aliases: [Experimental Serial Fraction, e]
tags: [performance, metrics, exam/formula]
sources: [lec27.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Karp-Flatt Metric

> [!abstract]
> The **Karp-Flatt Metric** ($e$) provides an **empirical measure** to extract the effective serial fraction from observed speedup. It is used to **diagnose the cause of poor scalability** by distinguishing between algorithmic serial limits and practical implementation overhead.

## Core Mechanics

> [!equation] The Karp-Flatt Formula
> $$e = \frac{1/S_p - 1/p}{1 - 1/p}$$
> - $S_p$: Observed speedup on $p$ processors.
> - $e$: Effective serial fraction (estimated).

### Interpretation of Results
- **Constant $e$ as $p$ increases**: Indicates that the speedup is limited primarily by the **inherent serial fraction** of the code ($f$ in Amdahl's Law). The algorithm itself is the bottleneck.
- **Increasing $e$ as $p$ increases**: Indicates that the speedup is limited by **parallel overhead**, such as communication costs, synchronization delays, or process management. This suggests the implementation or hardware is the bottleneck.

> [!warning] Amdahl vs Karp-Flatt
> While Amdahl's Law predicts a theoretical limit based on a given $f$, Karp-Flatt **diagnoses real implementations**, revealing overheads (like network latency) that the pure theoretical model usually ignores.

## Implementations & Examples

> [!example] Diagnostic Scenario
> If $S_2 = 1.8$, $S_4 = 3.0$, $S_8 = 4.7$:
> - $e_2 = (1/1.8 - 1/2) / (1 - 1/2) \approx 0.111$
> - $e_4 = (1/3.0 - 1/4) / (1 - 1/4) \approx 0.111$
> - $e_8 = (1/4.7 - 1/8) / (1 - 1/8) \approx 0.100$
> **Result**: $e$ is constant/decreasing, meaning the bottleneck is the **serial fraction of the algorithm**.

## Connections
* **Prerequisites:** [[parallel-speedup]], [[amdahls-law]].
* **Used In:** Performance tuning and bottleneck analysis.
* **Contrasts With:** [[parallel-efficiency]].
