#!/bin/bash
#SBATCH --job-name=idiot_pi
#SBATCH --partition=compute
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=8
#SBATCH --output=pi_final.out
#SBATCH --error=pi_final.err

# 1. Environment Setup (Always do this first!)
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

# 2. Network & MPI Handshake Fixes
export FI_PROVIDER=tcp
export FI_TCP_IFACE=10.0.0.0/24
export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi2.so.0

# 3. Execution
echo "Launch Time: $(date)"
srun --mpi=pmi2 ./pi_test
