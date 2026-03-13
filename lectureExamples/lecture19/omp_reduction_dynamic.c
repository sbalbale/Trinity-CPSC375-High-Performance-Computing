#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 10000

int main() {
    int arr[N];
    long sum = 0;

    // Initialize the array
    for (int i = 0; i < N; i++) {
        arr[i] = 1; // Filling array with 1s, sum should be 10000
    }

    double start = omp_get_wtime();

    // Parallel loop with dynamic scheduling and reduction
    #pragma omp parallel for schedule(dynamic) reduction(+:sum)
    for (int i = 0; i < N; i++) {
        sum += arr[i];
    }

    double end = omp_get_wtime();

    printf("Sum = %ld\n", sum);
    printf("Time = %f seconds\n", end - start);

    return 0;
}
