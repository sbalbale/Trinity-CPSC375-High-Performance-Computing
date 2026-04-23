---
aliases: [Comp/Comm Ratio, Arithmetic Intensity]
tags: [performance, scalability, theoretical]
sources: [lec31.txt, Homework 31.pdf]
created: 2026-04-20
updated: 2026-04-23
---

# Computation-to-Communication Ratio

> [!abstract]
> The **Computation-to-Communication Ratio** measures the amount of work (floating point operations) performed per unit of data moved. High ratios are critical for **scalable parallel algorithms** to hide network latency and memory bottlenecks.

## Core Mechanics

> [!equation] Matrix Multiplication Comparison
> For an $n \times n$ matrix on $p$ processors:
> - **1D Striped**: $\text{Ratio} \approx \frac{n^3/p}{n^2} = \frac{n}{p}$
> - **2D Checkerboard**: $\text{Ratio} \approx \frac{n^3/p}{n^2/\sqrt{p}} = \frac{n}{\sqrt{p}}$

### Interpretation
- **1D Striped**: As $p$ increases, the ratio drops quickly. To maintain a constant ratio (and thus constant efficiency), $n$ must grow linearly with $p$.
- **2D Checkerboard**: The ratio drops much more slowly ($\sqrt{p}$). To maintain a constant ratio, $n$ only needs to grow with $\sqrt{p}$. This makes 2D decomposition far more scalable.

> [!warning] The Memory Wall
> Even if the arithmetic intensity is high, if the communication bandwidth is low, the processors will remain idle while waiting for data.

## Connections
* **Prerequisites:** [[parallel-efficiency]].
* **Used In:** [[isoefficiency-function]], evaluating algorithm suitability for large clusters.
* **Contrasts With:** Work complexity (Big-O).
