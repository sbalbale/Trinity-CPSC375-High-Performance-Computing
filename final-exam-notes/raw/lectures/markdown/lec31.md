# Lecture 31: Matrix Multiplication II - Parallel Implementation

**Course:** CPSC 375: High-Performance Computing  
**Spring 2026** | Trinity College  
**Instructor:** Prof. Yoon

---

## Course Announcements

### Exam 2
- **Date:** Friday, April 24
- **Coverage:** Lectures 15-31
- **Format:**
  - Multiple-choice (closed-notes): terminology and basic concepts
  - Short-answer questions (open-notes): homework exercises and assignments

### Cluster Presentations
- **Date:** Wednesday, April 29
- **Format:** Lightning talk (7 minutes per team)
- **Topic:** Cluster projects and findings

### Term Project
- **Posted:** Wednesday, April 22
- **Due:** May 7

---

## Overview: Distributed Matrix Multiplication

This lecture covers parallel algorithms for matrix multiplication on distributed-memory clusters using MPI.

Key topics:
- Data distribution strategies
- Cannon's Algorithm
- Communication patterns
- Load balancing
- Performance optimization

---

## The Challenge: Naive Approach

### Problem

Compute $C = A \times B$ where each processor holds a partition of the matrices.

### Naive Algorithm Issues

```c
// NAIVE APPROACH - POOR COMMUNICATION
for (k = 0; k < n; k++) {
    for (i = 0; i < local_rows; i++) {
        for (j = 0; j < n; j++) {
            // Every iteration needs a[i][k] from all processors!
            // Every iteration needs entire column k of b
        }
    }
}
```

**Problems:**
- Each processor broadcasts entire rows/columns
- Communication: $O(n)$ messages of size $O(n)$ each
- Total: $O(n^2)$ communication
- Computation: $O(n^3/p)$ per processor

**Ratio:** Communication dominates for most practical scales!

---

## Cannon's Algorithm: Optimal Communication

### Key Idea

Use **systematic rotation** of submatrices so each processor receives exactly what it needs without global broadcast.

### Algorithm Overview

**Precondition:** Number of processors must be a perfect square ($p = q^2$)

**Setup:**
- Processor $(i, j)$ holds blocks: $A_{i,*}$ and $B_{*,j}$
- Grid of processors: $q \times q$ (where $p = q^2$)
- Block size: $(n/q) \times (n/q)$

### Cannon's Algorithm Steps

**Phase 1: Align matrices**

Initial arrangement: Processor $(i,j)$ has:
```
A block: A[i][0]
B block: B[0][j]
C block: 0 (initialized)
```

Need to align so processor $(i,j)$ computes $C[i][j]$:
- Rotate row $i$ of $A$ left by $i$ steps
- Rotate column $j$ of $B$ up by $j$ steps

After alignment:
- Processor $(i,j)$ has: $A[i][i]$ and $B[i][j]$
- Ready to do first $n/q \times n/q$ multiplication

**Phase 2: Compute with rotation**

```
for (step = 0; step < q; step++) {
    // Each processor multiplies its local blocks
    C_local += A_local × B_local;
    
    // Rotate A left (communicate with neighbor)
    A_local = receive from right;
    send A_local to left;
    
    // Rotate B up (communicate with neighbor)
    B_local = receive from below;
    send B_local to above;
}
```

**Result:** After $q$ steps, $C[i][j]$ is complete

### Example: 2×2 Processor Grid (4 Processors)

**Initial:**
```
P[0][0]: A[0][0], B[0][0]    P[0][1]: A[0][1], B[0][1]
P[1][0]: A[1][0], B[1][0]    P[1][1]: A[1][1], B[1][1]
```

**After alignment:**
```
P[0][0]: A[0][0], B[0][0]    P[0][1]: A[0][1], B[1][1]
P[1][0]: A[1][1], B[1][0]    P[1][1]: A[1][0], B[0][1]
```

**Step 1: Compute + Rotate**
```
P[0][0]: C += A[0][0] × B[0][0]  → A rotates in (A[0][1] comes from right)
P[0][1]: C += A[0][1] × B[1][1]  → A rotates in (A[0][0] comes from right)
```

**Step 2: Compute + Rotate**
```
P[0][0]: C += A[0][1] × B[1][0]  → all contributions accumulated ✓
P[0][1]: C += A[0][0] × B[0][1]  → all contributions accumulated ✓
```

### Cannon's Communication Pattern

**Per step:**
- $q-1$ rings of $q$ processors each
- Each processor sends/receives exactly one message
- Message size: $(n/q)^2$ elements = $O(n^2/p)$

**Total communication:**
- $q$ steps × message size = $O(q \times n^2/q^2) = O(n^2/q) = O(n^2/\sqrt{p})$

**Computation:**
- $q$ multiplications of $(n/q) \times (n/q)$ matrices = $O(n^3/p)$

**Ratio:** Computation-to-communication = $O(n/\sqrt{p})$

For good scalability, need $n >> \sqrt{p}$

---

## Communication Patterns in Distributed Matrix Multiplication

### Matrix A Distribution ("Communication of A")

Process $i$ needs specific blocks of $A$ that multiply with its blocks of $B$:

**Pattern 1: Broadcast by rows**
```
Row 0: Broadcast to all processors needing it
Row 1: Broadcast to all processors needing it
...
```

**Pattern 2: Cannon's rotation (systolic)**
```
Ring topology: each row rotates left
A flows in ring: faster than broadcast
```

### Matrix B Distribution ("Communication of B")

Similarly, $B$ must be distributed based on which processor needs it:

**Pattern 1: Broadcast by columns**
```
Column 0: Broadcast to all processors needing it
Column 1: Broadcast to all processors needing it
```

**Pattern 2: Cannon's rotation**
```
Ring topology: each column rotates up
B flows in ring
```

### Computing C

**Process $i$ computes $C_i = A_i^* \times B$**

Where $A_i^*$ refers to the specific rows of $A$ that process $i$ holds.

**Steps:**
1. Process $i$ computes $C_i = A_i^* \times B_i$ (local block)
2. Process $i$ sends current block of $B$ to neighbor
3. Process $i$ receives new block of $B$ from other neighbor
4. Repeat until all blocks have been multiplied

**Synchronization:** Barrier after each step to ensure all processes complete communication

---

## Ring Communication in Matrix Multiplication

### Systolic Array Concept

"Systolic" refers to rhythmic, wave-like communication:
- Like a heartbeat, data flows regularly through the system
- All processors synchronized
- Simple pattern: pass to neighbor

### Ring Topology

For process grid:
```
Horizontal rings (A distribution):
P[0][0] ← P[0][3] ← P[0][2] ← P[0][1] ← P[0][0]
P[1][0] ← P[1][3] ← P[1][2] ← P[1][1] ← P[1][0]
P[2][0] ← P[2][3] ← P[2][2] ← P[2][1] ← P[2][0]
P[3][0] ← P[3][3] ← P[3][2] ← P[3][1] ← P[3][0]

Vertical rings (B distribution):
  ↓       ↓       ↓       ↓
  ↑       ↑       ↑       ↑
```

### Advantages of Ring Topology

- ✓ Nearest-neighbor communication (faster on most clusters)
- ✓ Simple routing
- ✓ Reduces network congestion
- ✓ Scales better than all-to-all broadcast

---

## Load Balancing Issues

### Problem

Not all data distributions yield perfect load balance:

1. **Skewed block sizes**
   - When $n$ not divisible by $q$
   - Some processors have more data than others

2. **Computation depends on block size**
   - Processor with larger block does more work
   - Others must wait at barriers

### Solution Approaches

1. **Uneven distribution**
   - Allocate larger blocks to more processors
   - Balance load across processors

2. **Asynchronous algorithms**
   - Remove barriers for stragglers
   - Allow fast processors to continue
   - More complex to implement

3. **Pipeline parallelism**
   - Start next computation while waiting
   - Overlap communication and computation

---

## MPI Implementation

### Key MPI Calls for Cannon's Algorithm

**Process grid topology:**
```c
int dims[2] = {q, q};
int periodic[2] = {1, 1};  // Torus topology for ring wrap
MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, 0, &grid_comm);
MPI_Cart_coords(grid_comm, rank, 2, coords);  // Get (i,j)
```

**Shift for ring communication:**
```c
int left, right, up, down;
MPI_Cart_shift(grid_comm, 1, 1, &left, &right);    // Row shift (A)
MPI_Cart_shift(grid_comm, 0, 1, &up, &down);       // Column shift (B)
```

**Synchronous communication:**
```c
MPI_Sendrecv(send_buf, count, MPI_DOUBLE, neighbor_out, tag,
             recv_buf, count, MPI_DOUBLE, neighbor_in, tag,
             grid_comm, &status);  // Combines send + recv
```

**Barrier synchronization:**
```c
MPI_Barrier(grid_comm);  // All processes wait here
```

### Example: Cannon's Algorithm MPI Pseudocode

```c
// Assume A_block, B_block, C_block are local (n/q)×(n/q)
for (step = 0; step < q; step++) {
    // Local multiplication
    matrix_multiply(A_block, B_block, C_block, n/q);
    
    // Rotate A horizontally (left)
    MPI_Sendrecv(A_block, block_size, MPI_DOUBLE, left, A_TAG,
                 recv_A, block_size, MPI_DOUBLE, right, A_TAG,
                 grid_comm, &status);
    copy_matrix(recv_A, A_block, n/q);
    
    // Rotate B vertically (up)
    MPI_Sendrecv(B_block, block_size, MPI_DOUBLE, up, B_TAG,
                 recv_B, block_size, MPI_DOUBLE, down, B_TAG,
                 grid_comm, &status);
    copy_matrix(recv_B, B_block, n/q);
    
    // Implicit synchronization in MPI_Sendrecv
}
```

---

## Computation Visualization

### Computing C[i][j]

Each process $(i,j)$ accumulates:

$$C[i][j] = \sum_{k=0}^{q-1} A[i][k] \times B[k][j]$$

**In parallel:**
- Initially: has $A[i][i]$ and $B[i][j]$
- Step 0: $A[i][i] \times B[i][j]$ → contributes $A[i][i] \times B[i][j]$ to $C[i][j]$
- Step 1: receives $A[i][i+1]$ and $B[i+1][j]$ → contributes $A[i][i+1] \times B[i+1][j]$
- ...
- Step $q-1$: receives $A[i][i-1]$ and $B[i-1][j]$ → contributes $A[i][i-1] \times B[i-1][j]$

After $q$ steps: All $q$ terms accumulated → $C[i][j]$ complete ✓

---

## Performance Characteristics

### Speedup Analysis

**Computation:** $O(n^3/p)$ per processor

**Communication per step:** $O(n^2/\sqrt{p})$ (ring message size)

**Total communication:** $q \times O(n^2/\sqrt{p}) = O(n^2/\sqrt{p})$ (constant factor)

**Overall:**
$$S_p \approx \frac{n^3}{n^3/p + O(p^{1/4} \times n^2)} = \frac{n^3}{n^3/p + O(\sqrt{p} \times n^2)}$$

For large $n$: $S_p \approx p$ (linear speedup)
For fixed $n$: speedup plateaus as communication dominates

### Scalability Requirement

For good parallel efficiency:
$$\frac{\text{Computation}}{\text{Communication}} = \frac{n^3/p}{O(n^2/\sqrt{p})} = \frac{n}{\sqrt{p}} \gg 1$$

Need: $n \gg \sqrt{p}$ (problem must grow with processor count)

---

## Summary

**Cannon's Algorithm** achieves:
- ✓ Optimal communication: $O(n^2/\sqrt{p})$
- ✓ Balanced load: All processors do equal work
- ✓ Simple implementation: Ring topology, nearest-neighbor
- ✓ Scalable: Linear speedup for large $n$

**Key insight:** Systematic rotation avoids expensive broadcasts

---

## Exam Preparation

### Key Topics (Lectures 15-31)

1. **Parallel basics:** Speedup, Amdahl's/Gustafson's laws
2. **Sorting:** Bitonic, Odd-Even, Radix Sort
3. **Linear algebra:** Matrix multiplication optimization
4. **OpenMP:** Synchronization, critical sections
5. **MPI:** Distributed computing, collective operations
6. **Load balancing:** Understanding imbalance
7. **Communication patterns:** Broadcast, ring, all-to-all

### Exam Tips

- **Multiple choice:** Know definitions and laws
- **Short answer:** Prepare example problems
- **Bring notes:** For problem-solving questions
- **Practice:** Previous assignments

---

## Next Steps

1. Study for Exam 2 (Friday)
2. Prepare cluster presentation (Wednesday)
3. Watch for Term Project posting (Wednesday)
4. Final project due May 7

Good luck!
