#!/bin/bash
#SBATCH --job-name=radix_benchmark
#SBATCH --output=radix_results_%j.txt
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:10:00
#SBATCH --partition=defq

module load openmpi

# Build executables.
make clean
make all

echo "======================================================"
echo " Radix Sort Benchmarks"
echo " Date: $(date)"
echo "======================================================"

echo "--- Serial Baseline ---"
./RadixSerial 1000000 12345

echo "--- MPI Parallel (1 rank) ---"
srun --ntasks=1 ./RadixParallel 1000000 3 12345

echo "--- MPI Parallel (2 ranks) ---"
srun --ntasks=2 ./RadixParallel 1000000 3 12345

echo "--- MPI Parallel (4 ranks) ---"
srun --ntasks=4 ./RadixParallel 1000000 3 12345

echo "Benchmarks complete."
