#!/bin/bash
#SBATCH --job-name=mandelbrot_study
#SBATCH --partition=compute
#SBATCH --nodes=3
#SBATCH --ntasks=24
#SBATCH --output=mandelbrot_%j.out
#SBATCH --error=mandelbrot_%j.err
#SBATCH --time=02:00:00

echo "Launch Time: $(date)"

echo "Running scaling study across Three Idiots cluster..."
bash run_study.sh 24

echo "Job finished at: $(date)"