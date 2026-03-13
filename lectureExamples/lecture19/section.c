#include <stdio.h>
#include <omp.h>

int main()
{

#pragma omp parallel sections
    {
#pragma omp section
        printf("Section A executed by thread %d\n", omp_get_thread_num());

#pragma omp section
        printf("Section B executed by thread %d\n", omp_get_thread_num());

#pragma omp section
        printf("Section C executed by thread %d\n", omp_get_thread_num());
    }
}