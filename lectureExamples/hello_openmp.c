#include <stdio.h>
#include <omp.h>

int main() {
   #pragma omp parallel for
   for (int i=0; i<8; i++)
   {
      printf("Thread %d handles i=%d\n", omp_get_thread_num(), i);     
   }
}