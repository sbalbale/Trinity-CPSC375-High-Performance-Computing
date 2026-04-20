#!/bin/bash
#SBATCH --job-name=radix_benchmark
#SBATCH --partition=compute
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:10:00
#SBATCH --output=radix_results_%j.out
#SBATCH --error=radix_results_%j.err

# Optional cluster environment setup used in prior assignments.
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1
export FI_PROVIDER=tcp
export FI_TCP_IFACE=10.0.0.0/24
export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi2.so.0

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
srun --mpi=pmi2 -N 1 -n 1 ./RadixParallel 1000000 3 12345

echo "--- MPI Parallel (2 ranks) ---"
srun --mpi=pmi2 -N 1 -n 2 ./RadixParallel 1000000 3 12345

echo "--- MPI Parallel (4 ranks) ---"
srun --mpi=pmi2 -N 2 -n 4 ./RadixParallel 1000000 3 12345

echo "Benchmarks complete."
