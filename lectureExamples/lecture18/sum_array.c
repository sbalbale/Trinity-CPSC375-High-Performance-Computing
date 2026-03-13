#include <stdio.h>
#include <omp.h>

int main()
{
    int A[1000];
    long long sum = 0;

    // Initialize the array
    for (int i = 0; i < 1000; i++) {
        A[i] = i;
    }

    // Parallel sum using reduction
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < 1000; i++) {
        sum += A[i];
    }

    printf("Total Sum: %lld\n", sum);
    
    // Verification
    // Sum of 0 to N-1 is (N-1)*N/2. N=1000, 999*1000/2 = 499500
    printf("Expected Sum: %lld\n", 999LL * 1000LL / 2LL);

    return 0;
}
