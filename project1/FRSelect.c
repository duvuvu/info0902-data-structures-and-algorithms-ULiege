/* ========================================================================= *
 * frSelect
 * Rewritten implementation of the Floyd–Rivest Select algorithm
 * Based directly on the pseudocode specification
 * ========================================================================= */

#include <stddef.h>
#include <math.h>
#include "Select.h"

static int sign(double x) {
    if (x > 0)
        return 1;
    else if (x < 0)
        return -1;
    else
        return 0;
}

// Floyd-Rivest Select function
static size_t frSelect(void *array, size_t left, size_t right, size_t k,
                       int (*compare)(const void *, size_t, size_t),
                       void (*swap)(void *, size_t, size_t)) {
    while (right > left) {
        size_t n = right - left + 1;

        // Sample narrowing step if the range is large
        if (n > 600) {
            size_t i = k - left + 1;
            double z = log((double)n);
            double s = 0.5 * exp(2.0 * z / 3.0);
            double sd = 0.5 * sqrt(z * s * (n - s) / n) * sign(i - n / 2.0);

            size_t newLeft = (size_t)fmax(left, floor(k - i * s / n + sd));
            size_t newRight = (size_t)fmin(right, floor(k + (n - i) * s / n + sd));

            frSelect(array, newLeft, newRight, k, compare, swap);
        }

        // Partitioning
        size_t i = left;
        size_t j = right;
        size_t tIndex = k;
        swap(array, left, tIndex);
        tIndex = left;

        if (compare(array, right, tIndex) > 0)
            swap(array, left, right);

        while (i < j) {
            swap(array, i, j);
            i++;
            j--;

            while (i < right && compare(array, i, tIndex) < 0) i++;
            while (j > left && compare(array, j, tIndex) > 0) j--;
        }

        if (compare(array, left, tIndex) == 0) {
            swap(array, left, j);
        } else {
            j++;
            swap(array, j, right);
        }

        // Narrow the search interval
        if (j == k)
            return j;
        else if (j < k)
            left = j + 1;
        else
            right = j - 1;
    }

    return k;
}

// Initialize for FRSelect
size_t select(void *array, size_t length, size_t k,
              int (*compare)(const void *, size_t, size_t),
              void (*swap)(void *, size_t, size_t)) {
    if (length == 0 || k >= length)
        return 0;

    return frSelect(array, 0, length - 1, k, compare, swap);
}
