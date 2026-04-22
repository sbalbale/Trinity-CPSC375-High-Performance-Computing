# Homework 22 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1

**Problem:** You are developing a parallel simulation where each process needs to send its local state to a master node. The state consists of:
- A `double` representing the velocity
- A `float` representing the mass
- An `int` array of size 3 representing the 3D coordinates (x, y, z)

**A.** Define a new MPI derived datatype using `MPI_Type_create_struct`.  
**B.** Explain why you must use `MPI_Get_address` to find the displacements of each variable instead of simply using `sizeof()`.  
**C.** Write the `MPI_Type_commit` and `MPI_Type_free` sequence required to use and clean up this type.

---

### Part A — Define the MPI Struct Datatype

```c
#include <mpi.h>

typedef struct {
    double velocity;
    float  mass;
    int    coords[3];
} SimState;

void create_sim_state_type(SimState *state, MPI_Datatype *sim_type) {
    int          blocklengths[3]  = {1, 1, 3};
    MPI_Datatype types[3]         = {MPI_DOUBLE, MPI_FLOAT, MPI_INT};
    MPI_Aint     displacements[3];

    // Use MPI_Get_address to find actual memory addresses
    MPI_Aint base_addr;
    MPI_Get_address(state,               &base_addr);
    MPI_Get_address(&state->velocity,    &displacements[0]);
    MPI_Get_address(&state->mass,        &displacements[1]);
    MPI_Get_address(&state->coords[0],   &displacements[2]);

    // Convert absolute addresses to displacements relative to base
    displacements[0] -= base_addr;
    displacements[1] -= base_addr;
    displacements[2] -= base_addr;

    MPI_Type_create_struct(3, blocklengths, displacements, types, sim_type);
}
```

---

### Part B — Why `MPI_Get_address` Instead of `sizeof()`

Using `sizeof()` to compute displacements is **incorrect** because the C compiler is free to insert **padding bytes** between struct fields to satisfy alignment requirements. For example:

```
struct SimState {
    double velocity;   // 8 bytes (offset 0)
    float  mass;       // 4 bytes (offset 8)
    // [4 bytes padding here to align int to 4-byte boundary — may or may not exist]
    int    coords[3];  // 12 bytes (offset 12 or 16, depending on compiler/platform)
};
```

If you simply add up `sizeof(double) + sizeof(float)` to compute the offset of `coords`, you would get 12 — but the real offset might be 16 due to alignment padding. This would cause MPI to read from the wrong memory location, resulting in **data corruption**.

`MPI_Get_address` queries the **actual runtime memory address** of each field, so the computed displacements reflect the true layout including any compiler-inserted padding. This makes the derived type portable and correct regardless of architecture or compiler settings.

---

### Part C — `MPI_Type_commit` and `MPI_Type_free`

```c
// After creating the type, commit it before use:
MPI_Type_commit(sim_type);

// ... use sim_type in MPI_Send, MPI_Recv, etc. ...

// When done, free the type to release MPI resources:
MPI_Type_free(sim_type);
```

- **`MPI_Type_commit`** must be called before the type can be used in any communication call. It finalizes the type's internal representation within the MPI runtime.
- **`MPI_Type_free`** marks the type object for deallocation. After this call, the handle is set to `MPI_DATATYPE_NULL` and should not be used again.

---

## Question 2

**Problem:** Consider the following code used to time a parallel computation across a communicator `comm`:

```c
MPI_Barrier(comm);
start = MPI_Wtime();
/* Computation Block */
end = MPI_Wtime();
local_elapsed = end - start;
MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
```

**A.** Explain the specific purpose of calling `MPI_Barrier(comm)` before taking the start time.  
**B.** Why is `MPI_MAX` used in the `MPI_Reduce` function instead of `MPI_SUM` or `MPI_MIN`?  
**C.** If the barrier was removed, how would the reported elapsed time on the master node be affected?

---

### Part A — Purpose of `MPI_Barrier` Before the Timer

Without the barrier, processes may enter the computation block at **different real-world times**. A process that arrives early starts its timer, does some work, and finishes — but another process that was delayed (e.g., by OS scheduling, network contention, or the previous MPI call) might not start until much later.

`MPI_Barrier(comm)` **synchronizes all processes at a single point** before the clock starts. This ensures that `start = MPI_Wtime()` is called at roughly the same wall-clock moment by every process, so all elapsed time measurements reflect the *computation itself*, not startup skew. Without the barrier, you would be measuring the sum of scheduling delays plus computation time, which is not meaningful.

---

### Part B — Why `MPI_MAX` Instead of `MPI_SUM` or `MPI_MIN`

The goal is to measure the **total wall-clock time the parallel computation takes from start to finish** — i.e., the time the application must wait before all processes are done.

- **`MPI_MAX`** finds the elapsed time of the **slowest process**. Since the application cannot proceed until every process finishes, the actual total elapsed time is determined by the bottleneck process. This is the correct measure of parallel runtime.
- **`MPI_SUM`** would add up all processes' times, giving a meaningless aggregate (proportional to the number of processes, not wall-clock time).
- **`MPI_MIN`** would give the time of the **fastest process**, which understates the true elapsed time and ignores load imbalance.

---

### Part C — Effect of Removing the Barrier

Without `MPI_Barrier`, each process starts its timer independently. If some processes take longer to reach the start of the computation (due to prior work, OS scheduling, or network delays), they will have a **shorter measured `local_elapsed`** than a process that started earlier and ran the full computation.

The `MPI_Reduce` with `MPI_MAX` would then return the maximum of these skewed measurements. Depending on which process had the most *pre-computation* delay:

- **The reported time could be artificially inflated**: A fast process that started early records a long elapsed time because it waited longer in total wall-clock terms — but this includes idle waiting, not just computation.
- **The reported time could be artificially deflated**: If the slowest process also started latest (arriving at computation last), its timer started late and records a shorter elapsed time than the true wall-clock duration of the parallel computation.

In short, removing the barrier makes the timing measurement **unreliable and non-reproducible**, since it conflates scheduling jitter with computation time.
