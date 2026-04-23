#include <stdio.h>
#include <omp.h>

int main()
{

#pragma omp parallel
    {
        printf("Thread %d working\n", omp_get_thread_num());

#pragma omp master
        {
            printf("Master thread %d doing special work\n", omp_get_thread_num());
        }
    }
}