#include <stdio.h>
#include <omp.h>

int main()
{

#pragma omp parallel
    {
#pragma omp for nowait
        for (int i = 0; i < 8; i++)
        {
            printf("Thread %d loop iteration %d\n", omp_get_thread_num(), i);
        }
        printf("Thread %d continues immediately\n", omp_get_thread_num());
    }
}