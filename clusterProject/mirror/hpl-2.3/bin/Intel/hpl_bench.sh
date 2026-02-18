#!/bin/bash
#SBATCH --job-name=hpl_linpack
#SBATCH --partition=compute
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=8
#SBATCH --output=linpack.out
#SBATCH --error=linpack.err

source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

export FI_PROVIDER=tcp
export FI_TCP_IFACE=10.0.0.0/24
export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi2.so.0

echo "Starting HPL Benchmark..."
srun --mpi=pmi2 ./xhpl
echo "Benchmark Complete."
