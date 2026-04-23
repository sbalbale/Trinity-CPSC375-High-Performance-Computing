/*
 * File: RadixSerial.c
 * Purpose: A serial radix sort implementation.
 * Author: Sean Balbale
 * Date: 3/25/2026
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RADIX_BASE 256
#define RADIX_PASSES 4

static inline uint8_t extract_byte(uint32_t value, int pass) {
	// Extract one 8-bit digit for the current radix pass.
	return (uint8_t)((value >> (pass * 8)) & 0xFFu);
}

void counting_sort(uint32_t *input, uint32_t *output, int n, int pass) {
	// Stable counting sort for a single byte pass.
	int count[RADIX_BASE] = {0};
	int offset[RADIX_BASE];

	for (int i = 0; i < n; i++) {
		uint8_t digit = extract_byte(input[i], pass);
		count[digit]++;
	}

	offset[0] = 0;
	for (int digit = 1; digit < RADIX_BASE; digit++) {
		offset[digit] = offset[digit - 1] + count[digit - 1];
	}

	// Right-to-left placement preserves stability.
	for (int i = n - 1; i >= 0; i--) {
		uint8_t digit = extract_byte(input[i], pass);
		int pos = offset[digit] + count[digit] - 1;
		output[pos] = input[i];
		count[digit]--;
	}
}

void radix_sort(uint32_t *arr, int n) {
	if (n <= 1) {
		return;
	}

	uint32_t *buffer = (uint32_t *)malloc((size_t)n * sizeof(uint32_t));
	uint32_t *in = arr;
	uint32_t *out = buffer;

	if (buffer == NULL) {
		fprintf(stderr, "Error: failed to allocate radix sort buffer for %d items\n", n);
		exit(EXIT_FAILURE);
	}

	// Double-buffer by swapping input/output pointers each pass.
	for (int pass = 0; pass < RADIX_PASSES; pass++) {
		counting_sort(in, out, n, pass);

		uint32_t *tmp = in;
		in = out;
		out = tmp;
	}

	if (in != arr) {
		memcpy(arr, in, (size_t)n * sizeof(uint32_t));
	}

	free(buffer);
}

static int is_sorted_non_decreasing(const uint32_t *arr, int n) {
	for (int i = 1; i < n; i++) {
		if (arr[i - 1] > arr[i]) {
			return 0;
		}
	}
	return 1;
}

static uint32_t random_u32(void) {
	uint32_t a = (uint32_t)(rand() & 0xFFFF);
	uint32_t b = (uint32_t)(rand() & 0xFFFF);
	return (a << 16) | b;
}

int main(int argc, char **argv) {
	// Args: N [seed]
	int n = 1000000;
	unsigned int seed = (unsigned int)time(NULL);

	if (argc >= 2) {
		n = atoi(argv[1]);
	}
	if (argc >= 3) {
		seed = (unsigned int)strtoul(argv[2], NULL, 10);
	}

	if (n < 0) {
		fprintf(stderr, "Error: n must be non-negative\n");
		return EXIT_FAILURE;
	}

	uint32_t *data = (uint32_t *)malloc((size_t)n * sizeof(uint32_t));
	if (data == NULL) {
		fprintf(stderr, "Error: failed to allocate input array for %d items\n", n);
		return EXIT_FAILURE;
	}

	srand(seed);
	for (int i = 0; i < n; i++) {
		data[i] = random_u32();
	}

	struct timespec t0;
	struct timespec t1;
	if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0) {
		fprintf(stderr, "Error: clock_gettime start failed\n");
		free(data);
		return EXIT_FAILURE;
	}

	radix_sort(data, n);

	if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0) {
		fprintf(stderr, "Error: clock_gettime stop failed\n");
		free(data);
		return EXIT_FAILURE;
	}

	double elapsed =
		(double)(t1.tv_sec - t0.tv_sec) + (double)(t1.tv_nsec - t0.tv_nsec) / 1000000000.0;

	printf("N=%d seed=%u\n", n, seed);
	printf("sort_time_sec=%.6f\n", elapsed);
	printf("sorted=%s\n", is_sorted_non_decreasing(data, n) ? "yes" : "no");

	free(data);
	return EXIT_SUCCESS;
}
