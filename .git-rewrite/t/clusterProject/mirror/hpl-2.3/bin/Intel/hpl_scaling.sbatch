#!/bin/bash
#SBATCH --job-name=hpl_scaling
#SBATCH --partition=compute
#SBATCH --nodes=3
#SBATCH --ntasks=24
#SBATCH --time=00:30:00
#SBATCH --output=scaling_job_%j.log

RESULTS_FILE="scaling_results.txt"

# Clear the old file and start fresh
echo "======================================================" > $RESULTS_FILE
echo " CPSC 375: Three Idiots Cluster Scaling Benchmarks" >> $RESULTS_FILE
echo " Date: $(date)" >> $RESULTS_FILE
echo "======================================================" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# ---------------------------------------------------------
echo "Starting 1-Node Benchmark (8 Cores)..."
echo "--- 1 Node (8 Cores) ---" >> $RESULTS_FILE
cp HPL_1node.dat HPL.dat
srun -N 1 -n 8 --mpi=pmi2 ./xhpl | grep -A 2 "T/V" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# ---------------------------------------------------------
echo "Starting 2-Node Benchmark (16 Cores)..."
echo "--- 2 Nodes (16 Cores) ---" >> $RESULTS_FILE
cp HPL_2node.dat HPL.dat
srun -N 2 -n 16 --mpi=pmi2 ./xhpl | grep -A 2 "T/V" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# ---------------------------------------------------------
echo "Starting 3-Node Benchmark (24 Cores)..."
echo "--- 3 Nodes (24 Cores) ---" >> $RESULTS_FILE
cp HPL_3node.dat HPL.dat
srun -N 3 -n 24 --mpi=pmi2 ./xhpl | grep -A 2 "T/V" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

echo "======================================================" >> $RESULTS_FILE
echo "All benchmarks complete! Results saved to $RESULTS_FILE" >> $RESULTS_FILE
echo "Job finished."
