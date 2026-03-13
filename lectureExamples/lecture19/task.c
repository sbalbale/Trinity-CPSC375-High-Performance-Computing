#include <stdio.h>
#include <omp.h>

void work(int id)
{
    printf("Task %d executed by thread %d\n", id, omp_get_thread_num());
}

int main()
{
#pragma omp parallel
    {
#pragma omp single
        for (int i = 0; i < 8; i++)
        {
#pragma omp task
            work(i);
        }
    }
}