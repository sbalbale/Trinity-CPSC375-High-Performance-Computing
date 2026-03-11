#include <stdio.h>
#include <omp.h>

int main() {
   int sum = 0;

   #pragma omp parallel for reduction(+:sum)
   for (int i=0; i<1000; i++)
   {
      sum += 1;
   }
   printf("Sum = %d\n", sum);
}