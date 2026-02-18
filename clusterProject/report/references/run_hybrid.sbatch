#!/bin/bash
#SBATCH --job-name=hpl_hybrid
#SBATCH --partition=compute
#SBATCH --nodes=3
#SBATCH --ntasks=3
#SBATCH --cpus-per-task=8
#SBATCH --time=00:30:00
#SBATCH --output=hybrid_job_%j.log

RESULTS_FILE="hybrid_results.txt"

# Initialize the results file with a clean header
echo "======================================================" > $RESULTS_FILE
echo " CPSC 375: Three Idiots Cluster - Hybrid MPI + OpenMP" >> $RESULTS_FILE
echo " Date: $(date)" >> $RESULTS_FILE
echo "======================================================" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# 1. Configure the Hybrid Environment
export OMP_NUM_THREADS=8
export MKL_NUM_THREADS=8
export I_MPI_PIN_DOMAIN=omp

# 2. Swap the HPL Configuration
cp HPL_hybrid.dat HPL.dat

echo "Launching Hybrid HPL..." >> $RESULTS_FILE
echo "Nodes: 3 | MPI Ranks: 3 | Threads/Rank: 8" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# 3. Execute with MPI and PMI2
srun --mpi=pmi2 ./xhpl >> $RESULTS_FILE

echo "" >> $RESULTS_FILE
echo "======================================================" >> $RESULTS_FILE
echo "Hybrid benchmark complete!" >> $RESULTS_FILE
