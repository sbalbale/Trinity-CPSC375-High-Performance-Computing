# Lecture 15: Parallel Algorithm Design

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Parallel Algorithm Design

---

## Task/Channel Model

### Definition
A parallel computation is a set of tasks that interact through channels.

### Task Structure
- **Program:** Code executed by the task
- **Local Memory:** Private data storage for each task
- **I/O Ports:** Points of communication with other tasks
- **Interaction:** Tasks send messages through channels to communicate and synchronize

### Key Concept
Tasks are the fundamental units of parallel computation. Each task operates independently but communicates with other tasks through well-defined communication channels.

---

## Foster's Design Methodology

Foster's methodology provides a systematic approach to parallel algorithm design through four key stages:

1. **Partitioning** - Divide computation and data into tasks
2. **Communication** - Identify and characterize inter-task communication
3. **Agglomeration** - Group tasks to improve efficiency
4. **Mapping** - Assign tasks to processors

This methodology helps ensure that parallel algorithms are:
- Scalable
- Efficient
- Correct
- Implementable

### Design Goals
- Maximize concurrency by identifying as many independent tasks as possible
- Minimize communication overhead
- Balance computational load across processors
- Simplify implementation and maintenance

---

## Partitioning

### Definition
Dividing computation and data into pieces that can be distributed among processors.

### Two Main Approaches

#### 1. Domain Decomposition
- **Strategy:** Divide data into pieces first
- **Process:** Determine how to associate computations with the data
- **Best for:** Problems with natural spatial decomposition (images, grids, arrays)
- **Advantage:** Often leads to regular communication patterns
- **Example:** Image processing, finite element methods, stencil computations

#### 2. Functional Decomposition
- **Strategy:** Divide computation into pieces first
- **Process:** Determine how to associate data with the computations
- **Best for:** Problems with distinct computational phases (pipeline processing)
- **Advantage:** Can exploit different types of parallelism
- **Example:** Multi-stage pipelines, producer-consumer patterns

### Example: Image-Guided Brain Surgery
- Domain decomposition: Divide the 3D image into regions
- Each processor handles analysis and visualization of its region
- Functional decomposition: Different stages (image processing, reconstruction, visualization)

---

## Partitioning Checklist

When designing a partition, ensure:

1. **Maximum Task Count**
   - Identify at least an order of magnitude more primitive tasks than processors
   - Example: For a 16-processor system, aim for 160+ tasks
   - Benefit: Provides flexibility for load balancing and scalability

2. **Minimize Redundancy**
   - Minimize redundant computations across tasks
   - Minimize redundant data storage
   - Reduce memory footprint and computation waste

3. **Task Balance**
   - Primitive tasks are roughly the same size
   - Enables effective load balancing
   - Prevents processor idle time

4. **Scalability**
   - Number of tasks should increase as problem size increases
   - Ensures the algorithm scales to larger problems
   - Supports execution on systems with different processor counts

### Importance
A good partition forms the foundation for an efficient parallel algorithm. Poor partitioning decisions are difficult to recover from in later design stages.

---

## Communication

### Types of Communication Patterns

#### Local Communication
- **Pattern:** Task needs values from a small number of other tasks
- **Representation:** Create channels illustrating direct data flow
- **Characteristics:** Each task communicates with a few neighbors
- **Example:** Nearest-neighbor communication in grid computations

#### Global Communication
- **Pattern:** Significant number of tasks contribute data to perform a computation
- **Representation:** Don't create individual channels early in design
- **Characteristics:** Represents reduction, broadcast, or gather operations
- **Example:** Computing global sum, finding maximum value

### Key Insight
Communication introduces overhead that the sequential algorithm doesn't have. Therefore:
- Minimize communication volume
- Maximize communication locality
- Overlap communication with computation when possible

---

## Communication Checklist

Ensure your communication design has these properties:

1. **Load Balanced Communication**
   - Communication operations balanced among tasks
   - No single task becomes a communication bottleneck
   - Prevents tasks from waiting idly

2. **Sparse Communication**
   - Each task communicates with only a small group of neighbors
   - Reduces network congestion
   - Improves scalability
   - Maintains locality of reference

3. **Concurrent Communication**
   - Tasks can perform communications concurrently
   - Multiple pairs of tasks exchange data simultaneously
   - Reduces overall communication time
   - Utilizes full network bandwidth

4. **Concurrent Computation**
   - Tasks can perform computations concurrently
   - Communication and computation can overlap
   - Hides communication latency
   - Improves overall efficiency

### Trade-offs
- Fine-grained communication has lower latency but higher overhead
- Coarse-grained communication has higher latency but lower overhead
- Balance depends on target architecture and problem characteristics

---

## Agglomeration

### Definition
Grouping primitive tasks into larger tasks (agglomerated tasks) to reduce communication overhead and improve performance.

### Goals of Agglomeration

1. **Reduce Communication Overhead**
   - Eliminate communication between primitive tasks agglomerated together
   - Combine groups of sending and receiving tasks
   - Reduce the number of messages
   - Increase message size (better communication-to-computation ratio)

2. **Maintain Scalability**
   - Preserve the ability to scale to different processor counts
   - Can create different numbers of agglomerated tasks as needed
   - Supports both weak and strong scaling

3. **Reduce Engineering Cost**
   - Simpler implementation
   - Fewer tasks to manage
   - Clearer program structure
   - Easier debugging and testing

### How Agglomeration Improves Performance

#### Example: Before Agglomeration
```
Task A1 --send--> Task B1
Task A2 --send--> Task B2
Task A3 --send--> Task B3
```
- 3 separate send operations
- 3 separate receive operations
- High message count

#### Example: After Agglomeration
```
Agglomerated Task (A1+A2+A3) --send--> Agglomerated Task (B1+B2+B3)
```
- 1 combined send operation
- 1 combined receive operation
- Lower message count and overhead
- Better communication efficiency

### Practical Implementation
When developing MPI programs:
- Create one agglomerated task per processor
- Balance the load across processors
- Minimize communication between processors
- This is the bridge between the logical parallel design and the physical implementation

### Agglomeration Strategies

1. **Communication-Based Grouping**
   - Group tasks with frequent communication
   - Reduces inter-process communication

2. **Load-Based Grouping**
   - Group tasks to balance computational load
   - Ensures even processor utilization

3. **Memory-Based Grouping**
   - Group tasks sharing data structures
   - Improves cache locality and memory efficiency

---

## Summary: Design Methodology Flow

```
Start with problem
     ↓
Partitioning (Fine-grained)
     ↓
Communication Analysis
     ↓
Agglomeration (Coarse-grained)
     ↓
Mapping to processors
     ↓
Parallel implementation
```

### Key Takeaways

1. **Partitioning** creates a pool of fine-grained tasks (many more than processors)
2. **Communication** analysis identifies patterns and requirements
3. **Agglomeration** combines tasks to balance computation, communication, and simplicity
4. **Mapping** assigns agglomerated tasks to physical processors

This systematic approach leads to better parallel algorithms that are:
- Correct and reliable
- Efficient and scalable
- Maintainable and extensible
- Portable across different architectures

---

## References

- Foster, I. T. (1995). Designing and Building Parallel Programs
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
