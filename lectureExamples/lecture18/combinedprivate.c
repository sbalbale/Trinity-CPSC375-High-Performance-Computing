#include <stdio.h>
#include <omp.h>

int main()
{
    int x = 10;

#pragma omp parallel for firstprivate(x) lastprivate(x)
    for (int i = 0; i < 8; i++)
    {
        printf("Thread %d: starting x=%d\n", omp_get_thread_num(), x);
        x = i;
        printf("Thread %d: i=%d x=%d\n", omp_get_thread_num(), i, x);
    }
    printf("Final x = %d\n", x);
}
