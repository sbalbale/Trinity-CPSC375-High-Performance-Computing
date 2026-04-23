#include <stdio.h>
#include <omp.h>

int main()
{
    int counter = 0;

#pragma omp parallel for
    for (int i = 0; i < 1000; i++)
    {
#pragma omp atomic
        counter++;
    }
    printf("Counter = %d\n", counter);
}