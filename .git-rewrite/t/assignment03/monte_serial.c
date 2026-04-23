#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[])
{
    time_t start, end;
    long long number_of_tosses = 1000000; // Default
    long long number_in_circle = 0;
    long long toss;
    double x, y, distance_squared, pi_estimate;

    // Parse command-line argument for number of tosses
    if (argc > 1)
    {
        number_of_tosses = atoll(argv[1]);
    }

    printf("Tosses: %lld\n", number_of_tosses);

    start = time(NULL);

    srand(time(NULL)); // Seed random number generator

    for (toss = 0; toss < number_of_tosses; toss++)
    {
        // random double between -1 and 1
        x = (double)rand() / RAND_MAX * 2.0 - 1.0;
        y = (double)rand() / RAND_MAX * 2.0 - 1.0;
        distance_squared = x * x + y * y;
        if (distance_squared <= 1)
            number_in_circle++;
    }

    end = time(NULL);

    pi_estimate = 4 * number_in_circle / ((double)number_of_tosses);
    printf("Pi estimate: %f\n", pi_estimate);
    printf("Elapsed time = %ld seconds\n", end - start);

    return 0;
}
