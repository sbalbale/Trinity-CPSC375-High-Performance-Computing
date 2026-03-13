#include <stdio.h>
#include <omp.h>

int main()
{
    double start = omp_get_wtime();
    long sum = 0;

#pragma omp parallel for reduction(+ : sum)
    for (long i = 0; i < 100000000; i++)
    {
        sum += i;
    }
    double end = omp_get_wtime();

    printf("Sum = %ld\n", sum);
    printf("Time = %f seconds\n", end - start);
}