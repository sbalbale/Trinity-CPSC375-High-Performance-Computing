#!/bin/bash
#SBATCH --job-name=radix_benchmark
#SBATCH --output=radix_results_%j.txt
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:10:00
#SBATCH --partition=defq

module load openmpi

run_mpi() {
	local ranks="$1"
	shift

	# OpenMPI on Pine should be started with mpirun inside a SLURM allocation.
	if command -v mpirun >/dev/null 2>&1; then
		mpirun -np "$ranks" "$@"
	else
		echo "ERROR: mpirun not found after loading openmpi module." >&2
		exit 1
	fi
}

# Build executables.
make clean
make all

echo "======================================================"
echo " Radix Sort Benchmarks"
echo " Date: $(date)"
echo "======================================================"

echo "--- Serial Baseline ---"
./RadixSerial 1000000 12345

echo "--- MPI Parallel (1 rank) ---"
run_mpi 1 ./RadixParallel 1000000 3 12345

echo "--- MPI Parallel (2 ranks) ---"
run_mpi 2 ./RadixParallel 1000000 3 12345

echo "--- MPI Parallel (4 ranks) ---"
run_mpi 4 ./RadixParallel 1000000 3 12345

echo "Benchmarks complete."
