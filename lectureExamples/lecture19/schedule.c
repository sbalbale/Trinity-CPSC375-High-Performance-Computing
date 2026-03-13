#include <stdio.h>
#include <omp.h>

int main()
{

#pragma omp parallel for schedule(static)
    for (int i = 0; i < 16; i++)
    {
        printf("Thread %d executes iteration %d\n", omp_get_thread_num(), i);
    }
}