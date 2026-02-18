#!/bin/bash

# 1. Set the number of threads per MPI process
export OMP_NUM_THREADS=8
export MKL_NUM_THREADS=8

# 2. Tell Intel MPI to pin these threads efficiently to the physical cores
export I_MPI_PIN_DOMAIN=omp

echo "Starting Hybrid MPI + OpenMP Benchmark..."
echo "3 Nodes | 3 MPI Tasks | 8 Threads per Task"

# Swap in the hybrid config
cp HPL_hybrid.dat HPL.dat

# 3. Launch with the new Slurm allocation (-N 3 nodes, -n 3 tasks, -c 8 cores per task)
srun -p compute -N 3 -n 3 -c 8 --mpi=pmi2 ./xhpl
