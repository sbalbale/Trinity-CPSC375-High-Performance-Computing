---
aliases: [Mandelbrot, Dynamic Load Balancing Example]
tags: [algorithm, visualization, load-balancing]
sources: [lec16.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Mandelbrot Set Generation

> [!abstract]
> Generating the **Mandelbrot Set** is a computationally intensive task where the workload for each pixel is highly irregular. It is a classic example used to demonstrate the need for **dynamic mapping** and **load balancing** in parallel programming.

## Core Mechanics

### Workload Irregularity
- Some pixels escape the set quickly (low computation).
- Other pixels stay in the set for many iterations (high computation).
- The workload is not known beforehand, making [[static-mapping]] inefficient.

### Parallel Strategy
- **Master-Worker Pattern**: A master process manages a pool of tasks (e.g., rows of the image) and assigns them to workers as they become idle.
- **Dynamic Load Balancing**: This ensures that threads/processes that finish "easy" regions can immediately start on more difficult ones, minimizing idle time.

## Connections
* **Prerequisites:** [[master-worker-pattern]], [[dynamic-mapping]].
* **Used In:** Fractal generation, ray tracing.
* **Contrasts With:** Regular workloads (like basic matrix addition).
