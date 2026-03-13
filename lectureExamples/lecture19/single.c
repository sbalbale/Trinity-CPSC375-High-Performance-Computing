#include <stdio.h>
#include <omp.h>

int main()
{

#pragma omp parallel
    {
        printf("Thread %d running\n", omp_get_thread_num());

#pragma omp single
        {
            printf("One thread executes this section\n");
        }
    }
}