# ─────────────────────────────────────────────────────────────────────────────
# Makefile — MPI N-body Gravitational Simulator (Three Idiots Edition)
# ─────────────────────────────────────────────────────────────────────────────
CC      = mpiicx
CFLAGS  = -O3 -xHost -std=c99 -Wall -Wextra -pedantic
LDFLAGS = -lm
TARGET  = nbody
SRC     = nbody.c

.PHONY: all clean run study

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Quick sanity-check run: 2400 particles, 10 steps, seed 42, 8 processes via Slurm
run: $(TARGET)
	srun --mpi=pmi2 -n 8 ./$(TARGET) 2400 10 42

# Full scalability study (see run_study.sh)
study: $(TARGET)
	bash run_study.sh

clean:
	rm -f $(TARGET) timing_results.csv
