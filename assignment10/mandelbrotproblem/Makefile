# ─────────────────────────────────────────────────────────────────────────────
# Makefile — MPI Mandelbrot Set Generator (Three Idiots Edition)
# ─────────────────────────────────────────────────────────────────────────────
CC      = mpiicx
CFLAGS  = -O3 -xHost -std=c99 -Wall -Wextra -pedantic
LDFLAGS = -lm
TARGET  = mandelbrot
SRC     = mandelbrot.c

.PHONY: all clean run study

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Quick sanity-check run: 1024×1024, 1000 iterations, 24 processes via Slurm
run: $(TARGET)
	srun --mpi=pmi2 -n 24 ./$(TARGET) 1024 1024 1000 test_output.ppm

# Full scalability study (see run_study.sh)
study: $(TARGET)
	bash run_study.sh

clean:
	rm -f $(TARGET) *.ppm timing_results.csv