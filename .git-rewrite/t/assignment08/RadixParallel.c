/*
 * File: RadixParallel.c
 * Purpose: A parallel radix sort implementation using MPI.
 * Author: Sean Balbale
 * Date: 3/25/2026
 */

#include <inttypes.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RADIX_BASE 256
#define RADIX_PASSES 4

static inline uint8_t extract_byte(uint32_t value, int pass) {
	// Extract one 8-bit digit for the current pass.
	return (uint8_t)((value >> (pass * 8)) & 0xFFu);
}

static void compute_partition(uint64_t n_global, int p, int rank, uint64_t *start, int *count) {
	// Block partition with remainder spread over the first ranks.
	uint64_t base = n_global / (uint64_t)p;
	uint64_t rem = n_global % (uint64_t)p;

	uint64_t c = base + ((uint64_t)rank < rem ? 1u : 0u);
	uint64_t s = (uint64_t)rank * base + ((uint64_t)rank < rem ? (uint64_t)rank : rem);

	*start = s;
	*count = (int)c;
}

static int owner_of_global_index(uint64_t idx, const uint64_t *starts, int p) {
	// Binary search for the rank that owns global position idx.
	int lo = 0;
	int hi = p - 1;

	while (lo <= hi) {
		int mid = lo + (hi - lo) / 2;
		if (idx < starts[mid]) {
			hi = mid - 1;
		} else if (idx >= starts[mid + 1]) {
			lo = mid + 1;
		} else {
			return mid;
		}
	}

	return p - 1;
}

static void counting_sort_pass(const uint32_t *input, uint32_t *output, int n, int pass) {
	// Stable counting sort on a single byte (used for local ordering fixes).
	int count[RADIX_BASE] = {0};
	int offset[RADIX_BASE];

	for (int i = 0; i < n; i++) {
		uint8_t d = extract_byte(input[i], pass);
		count[d]++;
	}

	offset[0] = 0;
	for (int d = 1; d < RADIX_BASE; d++) {
		offset[d] = offset[d - 1] + count[d - 1];
	}

	// Right-to-left placement preserves stability.
	for (int i = n - 1; i >= 0; i--) {
		uint8_t d = extract_byte(input[i], pass);
		int pos = offset[d] + count[d] - 1;
		output[pos] = input[i];
		count[d]--;
	}
}

static int is_local_sorted(const uint32_t *arr, int n) {
	for (int i = 1; i < n; i++) {
		if (arr[i - 1] > arr[i]) {
			return 0;
		}
	}
	return 1;
}

static void fill_random(uint32_t *arr, int n, unsigned int seed) {
	srand(seed);
	for (int i = 0; i < n; i++) {
		uint32_t hi = (uint32_t)(rand() & 0xFFFF);
		uint32_t lo = (uint32_t)(rand() & 0xFFFF);
		arr[i] = (hi << 16) | lo;
	}
}

static void parallel_radix_sort(uint32_t **data_ptr, int *n_ptr, int rank, int p,
								const uint64_t *rank_starts) {
	// Four radix passes: histogram -> prefix/global counts -> shuffle -> local stable fix.
	uint32_t *data = *data_ptr;
	int n_local = *n_ptr;

	int local_hist[RADIX_BASE];
	int prefix_hist[RADIX_BASE];
	int global_hist[RADIX_BASE];
	uint64_t global_digit_start[RADIX_BASE];

	int *send_counts = (int *)malloc((size_t)p * sizeof(int));
	int *recv_counts = (int *)malloc((size_t)p * sizeof(int));
	int *send_displs = (int *)malloc((size_t)p * sizeof(int));
	int *recv_displs = (int *)malloc((size_t)p * sizeof(int));
	int *send_cursor = (int *)malloc((size_t)p * sizeof(int));
	int *target_for_elem = (int *)malloc((size_t)n_local * sizeof(int));

	if (send_counts == NULL || recv_counts == NULL || send_displs == NULL || recv_displs == NULL ||
		send_cursor == NULL || target_for_elem == NULL) {
		fprintf(stderr, "Rank %d failed to allocate communication buffers\n", rank);
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

	for (int pass = 0; pass < RADIX_PASSES; pass++) {
		// 1) Build local histogram for this digit.
		memset(local_hist, 0, sizeof(local_hist));
		for (int i = 0; i < n_local; i++) {
			uint8_t d = extract_byte(data[i], pass);
			local_hist[d]++;
		}

		// 2) Prefix counts from prior ranks; rank 0 is undefined and must be zeroed.
		MPI_Exscan(local_hist, prefix_hist, RADIX_BASE, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		if (rank == 0) {
			memset(prefix_hist, 0, sizeof(prefix_hist));
		}

		// 3) Global histogram defines digit block boundaries in global order.
		MPI_Allreduce(local_hist, global_hist, RADIX_BASE, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

		global_digit_start[0] = 0;
		for (int d = 1; d < RADIX_BASE; d++) {
			global_digit_start[d] = global_digit_start[d - 1] + (uint64_t)global_hist[d - 1];
		}

		memset(send_counts, 0, (size_t)p * sizeof(int));

		int seen_digit[RADIX_BASE] = {0};
		// 4) Map each local element to its owning rank by global position.
		for (int i = 0; i < n_local; i++) {
			uint8_t d = extract_byte(data[i], pass);
			uint64_t global_pos =
				global_digit_start[d] + (uint64_t)prefix_hist[d] + (uint64_t)seen_digit[d]++;
			int target = owner_of_global_index(global_pos, rank_starts, p);
			target_for_elem[i] = target;
			send_counts[target]++;
		}

		send_displs[0] = 0;
		for (int r = 1; r < p; r++) {
			send_displs[r] = send_displs[r - 1] + send_counts[r - 1];
		}

		memcpy(send_cursor, send_displs, (size_t)p * sizeof(int));

		uint32_t *send_buf = (uint32_t *)malloc((size_t)n_local * sizeof(uint32_t));
		if (send_buf == NULL) {
			fprintf(stderr, "Rank %d failed to allocate send buffer\n", rank);
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}

		for (int i = 0; i < n_local; i++) {
			int target = target_for_elem[i];
			int pos = send_cursor[target]++;
			send_buf[pos] = data[i];
		}

		// 5) Exchange counts, then payloads.
		MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);

		recv_displs[0] = 0;
		for (int r = 1; r < p; r++) {
			recv_displs[r] = recv_displs[r - 1] + recv_counts[r - 1];
		}

		int new_n_local = recv_displs[p - 1] + recv_counts[p - 1];
		uint32_t *recv_buf = (uint32_t *)malloc((size_t)new_n_local * sizeof(uint32_t));
		if (recv_buf == NULL) {
			fprintf(stderr, "Rank %d failed to allocate receive buffer\n", rank);
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}

		MPI_Alltoallv(send_buf, send_counts, send_displs, MPI_UINT32_T, recv_buf, recv_counts,
					  recv_displs, MPI_UINT32_T, MPI_COMM_WORLD);

		// 6) Stabilize local order for this pass after data arrives from many ranks.
		if (new_n_local > 1) {
			uint32_t *stable_fix = (uint32_t *)malloc((size_t)new_n_local * sizeof(uint32_t));
			if (stable_fix == NULL) {
				fprintf(stderr, "Rank %d failed to allocate local stable-fix buffer\n", rank);
				MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			}
			counting_sort_pass(recv_buf, stable_fix, new_n_local, pass);
			free(recv_buf);
			recv_buf = stable_fix;
		}

		free(send_buf);
		free(data);
		data = recv_buf;
		n_local = new_n_local;
	}

	free(send_counts);
	free(recv_counts);
	free(send_displs);
	free(recv_displs);
	free(send_cursor);
	free(target_for_elem);

	*data_ptr = data;
	*n_ptr = n_local;
}

int main(int argc, char **argv) {
	// Args: N_global [runs] [seed]
	MPI_Init(&argc, &argv);

	int rank = 0;
	int p = 1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	uint64_t n_global = (1ULL << 24);
	int runs = 3;
	unsigned int seed = 12345u;

	if (argc >= 2) {
		n_global = strtoull(argv[1], NULL, 10);
	}
	if (argc >= 3) {
		runs = atoi(argv[2]);
	}
	if (argc >= 4) {
		seed = (unsigned int)strtoul(argv[3], NULL, 10);
	}

	if (runs <= 0) {
		runs = 1;
	}

	uint64_t my_start = 0;
	int n_local_init = 0;
	compute_partition(n_global, p, rank, &my_start, &n_local_init);

	uint64_t *rank_starts = (uint64_t *)malloc((size_t)(p + 1) * sizeof(uint64_t));
	if (rank_starts == NULL) {
		fprintf(stderr, "Rank %d failed to allocate rank_starts\n", rank);
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

	for (int r = 0; r < p; r++) {
		uint64_t s = 0;
		int c = 0;
		compute_partition(n_global, p, r, &s, &c);
		rank_starts[r] = s;
	}
	rank_starts[p] = n_global;

	int *counts = NULL;
	int *displs = NULL;
	uint32_t *global_data = NULL;

	if (rank == 0) {
		counts = (int *)malloc((size_t)p * sizeof(int));
		displs = (int *)malloc((size_t)p * sizeof(int));
		if (counts == NULL || displs == NULL) {
			fprintf(stderr, "Rank 0 failed to allocate scatter metadata\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}

		for (int r = 0; r < p; r++) {
			uint64_t s = 0;
			int c = 0;
			compute_partition(n_global, p, r, &s, &c);
			counts[r] = c;
			displs[r] = (int)s;
		}

		global_data = (uint32_t *)malloc((size_t)n_global * sizeof(uint32_t));
		if (global_data == NULL) {
			fprintf(stderr, "Rank 0 failed to allocate global input\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}
	}

	double total_time = 0.0;

	for (int run = 0; run < runs; run++) {
		// Build fresh input each run on rank 0.
		if (rank == 0) {
			fill_random(global_data, (int)n_global, seed + (unsigned int)run);
		}

		uint32_t *local_data = (uint32_t *)malloc((size_t)n_local_init * sizeof(uint32_t));
		if (local_data == NULL) {
			fprintf(stderr, "Rank %d failed to allocate local data\n", rank);
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}

		MPI_Scatterv(global_data, counts, displs, MPI_UINT32_T, local_data, n_local_init,
					 MPI_UINT32_T, 0, MPI_COMM_WORLD);

		int n_local = n_local_init;

		// Time only sorting logic.
		MPI_Barrier(MPI_COMM_WORLD);
		double t0 = MPI_Wtime();

		parallel_radix_sort(&local_data, &n_local, rank, p, rank_starts);

		MPI_Barrier(MPI_COMM_WORLD);
		double t1 = MPI_Wtime();

		double elapsed = t1 - t0;
		double run_time = 0.0;
		MPI_Reduce(&elapsed, &run_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

		int local_ok = is_local_sorted(local_data, n_local);
		int global_ok = 0;
		MPI_Allreduce(&local_ok, &global_ok, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);

		uint32_t local_first = (n_local > 0) ? local_data[0] : 0;
		uint32_t local_last = (n_local > 0) ? local_data[n_local - 1] : 0;
		uint32_t prev_last = 0;
		int has_prev = (rank > 0) ? 1 : 0;

		if (rank > 0) {
			MPI_Recv(&prev_last, 1, MPI_UINT32_T, rank - 1, 999, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		if (rank < p - 1) {
			MPI_Send(&local_last, 1, MPI_UINT32_T, rank + 1, 999, MPI_COMM_WORLD);
		}

		int boundary_ok = 1;
		if (has_prev && n_local > 0 && prev_last > local_first) {
			boundary_ok = 0;
		}
		int boundaries_global_ok = 0;
		MPI_Allreduce(&boundary_ok, &boundaries_global_ok, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);

		if (rank == 0) {
			total_time += run_time;
			printf("run=%d time_sec=%.6f sorted_local=%s sorted_boundaries=%s\n", run + 1, run_time,
				   global_ok ? "yes" : "no", boundaries_global_ok ? "yes" : "no");
		}

		free(local_data);
	}

	if (rank == 0) {
		printf("N=%" PRIu64 " ranks=%d runs=%d avg_time_sec=%.6f\n", n_global, p, runs,
			   total_time / (double)runs);
	}

	free(rank_starts);
	if (rank == 0) {
		free(global_data);
		free(counts);
		free(displs);
	}

	MPI_Finalize();
	return 0;
}
