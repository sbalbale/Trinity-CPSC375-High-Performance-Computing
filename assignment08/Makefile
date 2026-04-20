# Compilers
CC = gcc
MPICC = mpicc

# Flags
CFLAGS = -O2 -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS =

# Binaries
SERIAL_BIN = RadixSerial
PARALLEL_BIN = RadixParallel

# Sources
SERIAL_SRC = RadixSerial.c
PARALLEL_SRC = RadixParallel.c

all: $(SERIAL_BIN) $(PARALLEL_BIN)

$(SERIAL_BIN): $(SERIAL_SRC)
	$(CC) $(CFLAGS) -o $(SERIAL_BIN) $(SERIAL_SRC) $(LDFLAGS)

$(PARALLEL_BIN): $(PARALLEL_SRC)
	$(MPICC) $(CFLAGS) -o $(PARALLEL_BIN) $(PARALLEL_SRC) $(LDFLAGS)

run-serial: $(SERIAL_BIN)
	./$(SERIAL_BIN)

run-parallel: $(PARALLEL_BIN)
	mpirun -np 4 ./$(PARALLEL_BIN)

clean:
	rm -f $(SERIAL_BIN) $(PARALLEL_BIN)

.PHONY: all run-serial run-parallel clean
