#include <stdio.h>
#include <omp.h>

int main() {
   int x = 5;

   #pragma omp parallel firstprivate(x)
   {
      x = x + 1;
      printf("Thread %d: x = %d\n", omp_get_thread_num(), x);
   }
   printf("Final x = %d\n", x);
}