#!/usr/bin/env bash
# =============================================================================
# run_study.sh — Strong & Weak Scaling Study for MPI Mandelbrot
# =============================================================================

# Disable strict mode so Intel's messy script doesn't crash us
set +eu 

# 1. Environment Setup for Three Idiots Cluster
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

# Turn basic strict mode back on (optional, but good practice)
set -e 

export FI_PROVIDER=tcp
export FI_TCP_IFACE=10.0.0.0/24
export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi2.so.0

BINARY="./mandelbrot"
RESULTS="timing_results.csv"
MAX_ITER=1000
TRIALS=3           
MAX_PROCS="${1:-24}" # Defaulting to full 24-core saturation

# ── Strong scaling parameters ────────────────────────────────────────────────
declare -a SS_WIDTHS=( 1024 2048 4096 )
declare -a SS_HEIGHTS=( 1024 2048 4096 )

# ── Weak scaling parameters ──────────────────────────────────────────────────
WEAK_BASE_W=512
WEAK_BASE_H=512

# ─────────────────────────────────────────────────────────────────────────────

if [[ ! -x "$BINARY" ]]; then
    echo "[error] Binary '$BINARY' not found. Run 'make' first." >&2
    exit 1
fi

if [[ ! -f "$RESULTS" ]]; then
    echo "study_type,width,height,max_iter,nprocs,trial,wall_time_s" > "$RESULTS"
fi

run_once() {
    local nprocs=$1 width=$2 height=$3 max_iter=$4
    local tmpout
    # Replaced mpirun with srun and pmi2 binding
    tmpout=$(srun --mpi=pmi2 -n "$nprocs" "$BINARY" "$width" "$height" "$max_iter" /dev/null 2>/dev/null)
    echo "$tmpout" | awk '/Wall time/ { print $4 }'
}

echo "=== Strong Scaling Study ==="
for idx in "${!SS_WIDTHS[@]}"; do
    W="${SS_WIDTHS[$idx]}"
    H="${SS_HEIGHTS[$idx]}"
    echo "  Problem: ${W}x${H}, max_iter=${MAX_ITER}"

    for nprocs in $(seq 2 "$MAX_PROCS"); do
        for trial in $(seq 1 "$TRIALS"); do
            t=$(run_once "$nprocs" "$W" "$H" "$MAX_ITER")
            echo "    P=${nprocs} trial=${trial} → ${t}s"
            echo "strong,${W},${H},${MAX_ITER},${nprocs},${trial},${t}" >> "$RESULTS"
        done
    done
done

echo ""
echo "=== Weak Scaling Study ==="
echo "  Base tile: ${WEAK_BASE_W}x${WEAK_BASE_H}, max_iter=${MAX_ITER}"

for nprocs in $(seq 2 "$MAX_PROCS"); do
    scale=$(python3 -c "import math; print(int(math.sqrt($nprocs)))")
    W=$(( WEAK_BASE_W * scale ))
    H=$(( WEAK_BASE_H * scale ))

    for trial in $(seq 1 "$TRIALS"); do
        t=$(run_once "$nprocs" "$W" "$H" "$MAX_ITER")
        echo "  P=${nprocs} size=${W}x${H} trial=${trial} → ${t}s"
        echo "weak,${W},${H},${MAX_ITER},${nprocs},${trial},${t}" >> "$RESULTS"
    done
done

echo ""
echo "Results written to: $RESULTS"
echo "Run 'python3 analyze.py' to compute speedup, efficiency, and generate plots."