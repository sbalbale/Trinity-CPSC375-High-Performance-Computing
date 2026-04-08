#!/bin/bash
#SBATCH --job-name=sieve_benchmark
#SBATCH --partition=compute
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --output=sieve_results.out
#SBATCH --error=sieve_results.err

# 1. Environment Setup (Replaces the missing 'module' command)
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

# 2. Network & MPI Handshake Fixes (Specific to your cluster)
export FI_PROVIDER=tcp
export FI_TCP_IFACE=10.0.0.0/24
export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi2.so.0

# 3. Execution
echo "======================================================"
echo " Sieve of Eratosthenes Benchmarks"
echo " Date: $(date)"
echo "======================================================"
echo ""

# Note: The assignment asks for 1, 2, and 4 processors. 
# We adjust -N (nodes) and -n (tasks/processors) to scale up.

echo "--- Running Domain Decomposition ---"
echo "1 Processor:"
srun --mpi=pmi2 -N 1 -n 1 ./domainSieve 100000000
echo "2 Processors:"
srun --mpi=pmi2 -N 1 -n 2 ./domainSieve 100000000
echo "4 Processors:"
srun --mpi=pmi2 -N 2 -n 4 ./domainSieve 100000000
echo ""

echo "--- Running Functional Decomposition ---"
echo "1 Processor:"
srun --mpi=pmi2 -N 1 -n 1 ./funcSieve 100000000
echo "2 Processors:"
srun --mpi=pmi2 -N 1 -n 2 ./funcSieve 100000000
echo "4 Processors:"
srun --mpi=pmi2 -N 2 -n 4 ./funcSieve 100000000
echo ""
echo "Benchmarks Complete!"
