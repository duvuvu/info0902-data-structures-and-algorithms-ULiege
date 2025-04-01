#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "IntArray.h"
#include "Select.h"

static int checkselect(int *array, size_t length, size_t k, size_t value)
{
    if (value >= length)
    {
        printf("   Something went wrong. The returned position (%zu) is not in the array.\n", value);
        return 0;
    }

    int key = array[value];
    size_t nblower = 0;
    size_t nbequal = 0;

    for (size_t i = 0; i < length; i++)
    {
        if (array[i] < key)
            nblower++;
        else if (array[i] == key)
            nbequal++;
    }

    if (k < nblower || k >= nblower + nbequal)
    {
        printf("   The returned index does not indicate the %zuth element in the array"
               " (between %zuth and %zuth).\n",
               k, nblower, nblower + nbequal - 1);
        return 0;
    }
    return 1;
}

static inline double selectCPUTime(int *array, size_t length, size_t k)
{
    clock_t start = clock();
    select(array, length, k, compareInt, swapInt);
    return ((double)(clock() - start)) / CLOCKS_PER_SEC;
}

// int main(int argc, char **argv)
// {
//     srand(time(NULL));

//     // Arguments
//     size_t n = 10000, k = n / 2;
//     const size_t n_checktest = 1000;

//     if (argc > 1)
//         sscanf(argv[1], "%zu", &n);
//     if (argc > 2)
//         sscanf(argv[2], "%zu", &k);

//     // Tests

//     printf("Testing the select function for all values of k on a random array of length 1000...\n");

//     int *array = fillRandomArray(NULL, n_checktest);
//     int *copyarray = copyArray(array, n_checktest, NULL);

//     for (size_t i = 0; i < n_checktest; i++)
//     {
//         size_t pos = select(array, n_checktest, i, compareInt, swapInt);
//         if (!checkselect(array, n_checktest, i, pos))
//         {
//             printf("   k = %zu did not return the expected results. Stopping the tests.\n", i);
//             exit(-1);
//         }
//         copyArray(copyarray, n_checktest, array);
//     }

//     free(array);
//     free(copyarray);

//     printf("=> The select function seems correct.\n\nComputing CPU time and numbers of swaps and comparisons.\n\n");

//     printf("Array         Time [s]   (#swaps,#comps)\n");
//     printf("------------  --------   -----------------------\n");

//     array = fillIncreasingArray(NULL, n);
//     resetNbSwaps();
//     resetNbComparisons();
//     float cputime = selectCPUTime(array, n, k);
//     printf("increasing    %f\t  (%zu,%zu)\n", cputime, getNbSwaps(), getNbComparisons());

//     fillDecreasingArray(array, n);
//     resetNbSwaps();
//     resetNbComparisons();
//     cputime = selectCPUTime(array, n, k);
//     printf("decreasing    %f\t  (%zu,%zu)\n", cputime, getNbSwaps(), getNbComparisons());

//     fillRandomArray(array, n);
//     resetNbSwaps();
//     resetNbComparisons();
//     cputime = selectCPUTime(array, n, k);
//     printf("random        %f\t  (%zu,%zu)\n", cputime, getNbSwaps(), getNbComparisons());

//     fillConstantArray(array, n);
//     resetNbSwaps();
//     resetNbComparisons();
//     cputime = selectCPUTime(array, n, k);
//     printf("constant      %f\t  (%zu,%zu)\n", cputime, getNbSwaps(), getNbComparisons());

//     free(array);

//     return 0;
// }

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "IntArray.h"
#include "Select.h"

#define NUM_RUNS 10

typedef int *(*FillFunction)(int *, size_t);

// Function to run an experiment and return average time over multiple runs
double averageRunTime(FillFunction fillFn, size_t n, size_t k) {
    double total_time = 0;

    for (int run = 0; run < NUM_RUNS; run++) {
        int *array = fillFn(NULL, n);
        resetNbSwaps();
        resetNbComparisons();

        clock_t start = clock();
        select(array, n, k, compareInt, swapInt);
        clock_t end = clock();

        total_time += (double)(end - start) / CLOCKS_PER_SEC;

        free(array);
    }

    return total_time / NUM_RUNS;
}

size_t averageSwaps(FillFunction fillFn, size_t n, size_t k) {
    size_t total_swaps = 0;

    for (int run = 0; run < NUM_RUNS; run++) {
        int *array = fillFn(NULL, n);
        resetNbSwaps();
        resetNbComparisons();

        select(array, n, k, compareInt, swapInt);

        total_swaps += getNbSwaps();
        free(array);
    }

    return total_swaps / NUM_RUNS;
}

size_t averageComparisons(FillFunction fillFn, size_t n, size_t k) {
    size_t total_comps = 0;

    for (int run = 0; run < NUM_RUNS; run++) {
        int *array = fillFn(NULL, n);
        resetNbSwaps();
        resetNbComparisons();

        select(array, n, k, compareInt, swapInt);

        total_comps += getNbComparisons();
        free(array);
    }

    return total_comps / NUM_RUNS;
}

void runExperiment(size_t n, size_t k, const char *label) {
    FillFunction types[] = {fillRandomArray, fillIncreasingArray, fillDecreasingArray, fillConstantArray};
    const char *typeNames[] = {"random", "increasing", "decreasing", "constant"};

    printf("Results for %s (k = %zu)\n", label, k);
    printf("%-12s  Time [s]   (#swaps, #comps)\n", "Array");
    printf("------------  --------   ----------------------\n");

    for (int i = 0; i < 4; i++) {
        double avg_time = averageRunTime(types[i], n, k);
        size_t avg_swaps = averageSwaps(types[i], n, k);
        size_t avg_comps = averageComparisons(types[i], n, k);

        printf("%-12s  %8.5f   (%7zu, %7zu)\n", typeNames[i], avg_time, avg_swaps, avg_comps);
    }

    printf("\n");
}

int main(int argc, char **argv) {
    size_t n = 10000;
    if (argc > 1)
        sscanf(argv[1], "%zu", &n);

    size_t k_median = n / 2 - 1;
    size_t k_percentile = n / 10 - 1;

    printf("Running experiments on array size: %zu\n", n);
    printf("Each result is averaged over %d runs.\n\n", NUM_RUNS);

    runExperiment(n, k_median, "Median");
    runExperiment(n, k_percentile, "10th Percentile");

    return 0;
}