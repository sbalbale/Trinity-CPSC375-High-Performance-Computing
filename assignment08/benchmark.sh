#!/bin/bash
#SBATCH --job-name=radix-benchmark
#SBATCH --output=radix_results_%j.txt
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=24
#SBATCH --time=01:30:00
#SBATCH --partition=defq

# =============================================================================
# RADIX SORT BENCHMARKING SCRIPT - Pine Cluster
# Part III: Scaling Test and Performance Analysis
# Dataset size: N = 2^28 (approximately 268 million elements)
# =============================================================================

module load openmpi

# Build executables
make clean
make all

# Configuration
DATASET_SIZE=$((2**28))  # 2^28 integers
NUM_RUNS=3               # Number of times to run each configuration
SEED=42                  # Random seed for reproducibility

echo "==============================================================================="
echo "RADIX SORT PARALLEL IMPLEMENTATION - BENCHMARKING RESULTS"
echo "==============================================================================="
echo "Pine Cluster Strong Scaling Analysis"
echo "Date: $(date)"
echo "Dataset Size: N = $DATASET_SIZE (2^28 integers ≈ 268 million elements)"
echo "Number of Runs per Configuration: $NUM_RUNS"
echo "Random Seed: $SEED"
echo ""
echo "Test Configurations:"
echo "  Test I:   1 rank (Serial baseline)"
echo "  Test II:  12 ranks (Single socket / Shared L3)"
echo "  Test III: 24 ranks (Dual socket / QPI)"
echo "  Test IV:  48 ranks (InfiniBand / 2 nodes)"
echo "  Test V:   96 ranks (Standard test / 4 nodes)"
echo "==============================================================================="
echo ""

# Test I: Serial baseline (1 rank)
echo "Test I: Serial Baseline (1 rank)"
echo "---"
for ((run=1; run<=NUM_RUNS; run++)); do
    echo "Run $run:"
    srun --ntasks=1 ./RadixParallel $DATASET_SIZE $NUM_RUNS $SEED
done
echo ""

# Test II: Single socket (12 ranks)
echo "Test II: Single Socket (12 ranks)"
echo "---"
for ((run=1; run<=NUM_RUNS; run++)); do
    echo "Run $run:"
    srun --ntasks=12 ./RadixParallel $DATASET_SIZE $NUM_RUNS $SEED
done
echo ""

# Test III: Dual socket (24 ranks)
echo "Test III: Dual Socket (24 ranks)"
echo "---"
for ((run=1; run<=NUM_RUNS; run++)); do
    echo "Run $run:"
    srun --ntasks=24 ./RadixParallel $DATASET_SIZE $NUM_RUNS $SEED
done
echo ""

# Test IV: Distributed 2 nodes (48 ranks)
echo "Test IV: Distributed (48 ranks / 2 nodes)"
echo "---"
for ((run=1; run<=NUM_RUNS; run++)); do
    echo "Run $run:"
    srun --ntasks=48 ./RadixParallel $DATASET_SIZE $NUM_RUNS $SEED
done
echo ""

# Test V: Standard test 4 nodes (96 ranks)
echo "Test V: Standard Test (96 ranks / 4 nodes)"
echo "---"
for ((run=1; run<=NUM_RUNS; run++)); do
    echo "Run $run:"
    srun --ntasks=96 ./RadixParallel $DATASET_SIZE $NUM_RUNS $SEED
done
echo ""

echo "==============================================================================="
echo "Benchmarking Complete"
echo "Date: $(date)"
echo "==============================================================================="
echo ""
echo "Performance Metrics to Calculate:"
echo "1. Speedup (S_p) = T_serial / T_p"
echo "2. Efficiency (E_p) = (S_p / p) × 100%"
echo "   where p = number of ranks, T_serial = Test I time, T_p = execution time with p ranks"
echo ""
echo "Results saved in output file: radix_results_\$SLURM_JOB_ID.txt"
