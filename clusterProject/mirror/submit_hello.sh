#!/bin/bash
#SBATCH --job-name=mpi_test
#SBATCH --output=result_%j.out
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=8
#SBATCH --partition=compute

# Load the environment (just in case)
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

# Run the program
mpirun ./hello_mpi
