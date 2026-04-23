#!/bin/bash
#SBATCH --job-name=pine_sieve_benchmark
#SBATCH --output=pine_sieve_results_%j.txt
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:15:00
#SBATCH --partition=defq 

module load openmpi

echo "--- Running Domain Decomposition ---"
echo "1 Processor:"
srun --ntasks=1 ./domainSieve 100000000
echo "2 Processors:"
srun --ntasks=2 ./domainSieve 100000000
echo "4 Processors:"
srun --ntasks=4 ./domainSieve 100000000

echo "--- Running Functional Decomposition ---"
echo "1 Processor:"
srun --ntasks=1 ./funcSieve 100000000
echo "2 Processors:"
srun --ntasks=2 ./funcSieve 100000000
echo "4 Processors:"
srun --ntasks=4 ./funcSieve 100000000
