---
aliases:
  - Skew-and-Shift Matrix Mult
tags:
  - algorithm
  - matrix
  - distributed
  - mpi
sources:
  - lec31.txt
  - Homework 31.pdf
created: 2026-04-20
updated: 2026-04-23
---

# Cannon's Algorithm

> [!abstract]
> **Cannon's Algorithm** is a distributed matrix multiplication algorithm for 2D meshes. It uses an **initial skewing** alignment phase followed by a series of **shift-multiply** steps to compute the product in $O(n^3/p)$ time with $O(n/\sqrt{p})$ communication ratio.

## Core Mechanics

### Cartesian Topology Setup
Before implementing the shifts, MPI must define a 2D process grid (torus) to handle the circular wrap-around automatically.

> [!code] MPI Grid Creation
> ```c
> int dims[2] = {q, q};       // q = sqrt(p)
> int periodic[2] = {1, 1};   // 1 = periodic (wrap-around)
> MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, 0, &grid_comm);
> // Find neighbors for shifting
> int left, right, up, down;
> MPI_Cart_shift(grid_comm, 1, 1, &left, &right); // Row shift (A)
> MPI_Cart_shift(grid_comm, 0, 1, &up, &down);    // Col shift (B)
> ```

### Phase 1: Alignment (Skewing)
Before multiplication starts, blocks must be aligned so that each processor has a valid pair of submatrices to multiply:
- **Matrix A**: Circularly shift each row $i$ left by $i$ positions.
- **Matrix B**: Circularly shift each column $j$ up by $j$ positions.

### Phase 2: Shift-Multiply (Rotation)
Perform $q = \sqrt{p}$ iterations of computation and neighbor-to-neighbor communication.

> [!code] Rotation Loop
> ```c
> for (step = 0; step < q; step++) {
>     C_local += A_local * B_local;
>     // Rotate A left, B up simultaneously
>     MPI_Sendrecv(A_local, size, MPI_DOUBLE, left, TAG, recv_A, size, MPI_DOUBLE, right, TAG, grid_comm, &status);
>     MPI_Sendrecv(B_local, size, MPI_DOUBLE, up, TAG, recv_B, size, MPI_DOUBLE, down, TAG, grid_comm, &status);
>     A_local = recv_A; B_local = recv_B;
> }
> ```

> [!warning] Implementation Complexity
> Cannon's is often difficult to implement due to the complex circular shifting logic required for the initial alignment, especially if the number of processors is not a perfect square ($p = q^2$).

## Performance Analysis
- **Communication per step**: Each process sends/receives one block of size $(n/q)^2 = O(n^2/p)$.
- **Total Communication**: $q \times O(n^2/p) = O(n^2/\sqrt{p})$.
- **Scalability**: Achieving linear speedup requires $n \gg \sqrt{p}$.

## Connections
* **Prerequisites:** [[checkerboard-decomposition]].
* **Used In:** Distributed-memory matrix multiplication.
* **Contrasts With:** [[foxs-algorithm]] (broadcast instead of skew).
