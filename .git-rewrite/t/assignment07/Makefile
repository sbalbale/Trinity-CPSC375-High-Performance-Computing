# MPI compiler and flags
CC = mpicc
CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -lm

# Default target
all: domainSieve funcSieve

# Build domain decomposition version
domainSieve: domainSieve.c
	$(CC) $(CFLAGS) -o domainSieve domainSieve.c $(LDFLAGS)

# Build functional decomposition version
funcSieve: funcSieve.c
	$(CC) $(CFLAGS) -o funcSieve funcSieve.c $(LDFLAGS)

# Remove generated binaries
clean:
	rm -f domainSieve funcSieve

.PHONY: all clean
