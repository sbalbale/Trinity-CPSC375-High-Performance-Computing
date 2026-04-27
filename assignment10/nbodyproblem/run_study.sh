#!/usr/bin/env bash
# =============================================================================
# run_study.sh — Strong & Weak Scaling Study for MPI N-body Simulator
# =============================================================================
#
# Strong scaling: fixed N, P swept from 1 to MAX_PROCS.
#   N = 2400, 4800, 9600 particles; NSTEPS_STRONG = 10 timesteps.
#
# Weak scaling: N grows proportionally with P (N = WEAK_BASE * P),
#   keeping particles-per-process constant at WEAK_BASE.
#   NSTEPS_WEAK = 50 timesteps. Note: because the all-pairs algorithm is
#   O(N²), each process's work still grows as O(N) = O(P·base), so
#   normalized wall time is expected to rise linearly with P — this
#   demonstrates the fundamental scalability ceiling of O(N²) methods.

set +eu

# 1. Environment Setup for Three Idiots Cluster
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

set -e

export FI_PROVIDER=tcp
export FI_TCP_IFACE=10.0.0.0/24
export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi2.so.0

BINARY="./nbody"
RESULTS="timing_results.csv"
SEED=42
TRIALS=3
MAX_PROCS="${1:-24}"

# ── Strong scaling parameters ─────────────────────────────────────────────────
declare -a SS_N=( 2400 4800 9600 )
NSTEPS_STRONG=10

# ── Weak scaling parameters ───────────────────────────────────────────────────
# Each rank always owns WEAK_BASE particles; total N = WEAK_BASE * nprocs.
WEAK_BASE=1000
NSTEPS_WEAK=50

# ─────────────────────────────────────────────────────────────────────────────

if [[ ! -x "$BINARY" ]]; then
    echo "[error] Binary '$BINARY' not found. Run 'make' first." >&2
    exit 1
fi

if [[ ! -f "$RESULTS" ]]; then
    echo "study_type,N,nsteps,nprocs,trial,wall_time_s" > "$RESULTS"
fi

run_once() {
    local nprocs=$1 N=$2 nsteps=$3
    local tmpout
    tmpout=$(srun --mpi=pmi2 -n "$nprocs" "$BINARY" "$N" "$nsteps" "$SEED" 2>/dev/null)
    echo "$tmpout" | awk '/Wall time/ { print $4 }'
}

echo "=== Strong Scaling Study ==="
for N in "${SS_N[@]}"; do
    echo "  N=${N}, nsteps=${NSTEPS_STRONG}"
    for nprocs in $(seq 1 "$MAX_PROCS"); do
        for trial in $(seq 1 "$TRIALS"); do
            t=$(run_once "$nprocs" "$N" "$NSTEPS_STRONG")
            echo "    P=${nprocs} trial=${trial} → ${t}s"
            echo "strong,${N},${NSTEPS_STRONG},${nprocs},${trial},${t}" >> "$RESULTS"
        done
    done
done

echo ""
echo "=== Weak Scaling Study ==="
echo "  Base per process: ${WEAK_BASE} particles, nsteps=${NSTEPS_WEAK}"

for nprocs in $(seq 1 "$MAX_PROCS"); do
    N=$(( WEAK_BASE * nprocs ))
    for trial in $(seq 1 "$TRIALS"); do
        t=$(run_once "$nprocs" "$N" "$NSTEPS_WEAK")
        echo "  P=${nprocs} N=${N} trial=${trial} → ${t}s"
        echo "weak,${N},${NSTEPS_WEAK},${nprocs},${trial},${t}" >> "$RESULTS"
    done
done

echo ""
echo "Results written to: $RESULTS"
echo "Run 'python3 analyze.py' to compute speedup, efficiency, and generate plots."
