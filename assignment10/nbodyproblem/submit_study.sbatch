#!/bin/bash
#SBATCH --job-name=nbody_study
#SBATCH --partition=compute
#SBATCH --nodes=3
#SBATCH --ntasks=24
#SBATCH --output=nbody_%j.out
#SBATCH --error=nbody_%j.err
#SBATCH --time=02:00:00

echo "Launch Time: $(date)"

echo "Running N-body scaling study across Three Idiots cluster..."
bash run_study.sh 24

echo "Job finished at: $(date)"
