---
aliases: [Strassen's Multiplication, $O(n^{2.81})$ Matrix Mult]
tags: [algorithm, matrix, divide-and-conquer]
sources: [lec30.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Strassen's Algorithm

> [!abstract]
> **Strassen's Algorithm** is a divide-and-conquer matrix multiplication method that reduces the algebraic complexity from $O(n^3)$ to **$O(n^{2.81})$**. It does this by reducing the number of scalar multiplications required for a $2 \times 2$ matrix multiplication from 8 to **7**.
## Core Mechanics

### Strassen's Formulas (for 2x2)
Instead of 8 multiplications, Strassen uses 7 intermediate products ($m_1$ through $m_7$):
> [!code] Intermediate Products
> ```
> m1 = (A[0][0] + A[1][1]) * (B[0][0] + B[1][1])
> m2 = (A[1][0] + A[1][1]) * B[0][0]
> m3 = A[0][0] * (B[0][1] - B[1][1])
> m4 = A[1][1] * (B[1][0] - B[0][0])
> m5 = (A[0][0] + A[0][1]) * B[1][1]
> m6 = (A[1][0] - A[0][0]) * (B[0][0] + B[0][1])
> m7 = (A[0][1] - A[1][1]) * (B[1][0] + B[1][1])
> ```

### Result Construction
The final components of $C$ are computed using only additions and subtractions:
> [!code] Final Result
> ```
> C[0][0] = m1 + m4 - m5 + m7
> C[0][1] = m3 + m5
> C[1][0] = m2 + m4
> C[1][1] = m1 - m2 + m3 + m6
> ```

> [!equation] Time Complexity
> $$T(n) = 7 T(n/2) + O(n^2) \implies O(n^{\log_2 7}) \approx O(n^{2.81})$$

> [!warning] Trade-offs
> - **Additions**: Strassen's requires significantly more additions (**18 vs 4**).
> - **Numerical Stability**: It is less numerically stable than the standard algorithm.
> - **Crossover Point**: Typically only faster for matrices larger than **500x500**.

## Connections
...
* **Prerequisites:** [[shared-memory-model]] (Divide-and-Conquer).
* **Used In:** Large-scale linear algebra where multiplication is the dominant cost.
* **Contrasts With:** Standard $O(n^3)$ multiplication.
* **Author:** [[strassen]].
