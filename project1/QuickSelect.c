/* ========================================================================= *
 * QuickSelect
 * Implementation of the QuickSelect algorithm
 * ========================================================================= */

 #include <stddef.h>
 #include "Select.h"

// Partition function
static size_t partition(void *array, size_t p, size_t r,
                        int (*compare)(const void *, size_t, size_t),
                        void (*swap)(void *, size_t, size_t))
{
    size_t pivotIndex = r;
    size_t i = p - 1;

    for (size_t j = p; j < r; j++) {
        if (compare(array, j, pivotIndex) <= 0) {
            i++;
            swap(array, i, j);
        }
    }

    swap(array, i + 1, pivotIndex);
    
    return i + 1;
}

// Quick Select function
static size_t quickSelect(void *array, size_t p, size_t r, size_t k,
                        int (*compare)(const void *, size_t, size_t),
                        void (*swap)(void *, size_t, size_t))
{
    if (p == r)
        return p;

    size_t q = partition(array, p, r, compare, swap);

    if (k == q)
        return q;
    else if (k < q)
        return quickSelect(array, p, q - 1, k, compare, swap);
    else
        return quickSelect(array, q + 1, r, k, compare, swap);
}

// Initializer for QuickSelect
size_t select(void *array, size_t length, size_t k,
            int (*compare)(const void *, size_t, size_t),
            void (*swap)(void *, size_t, size_t))
{
    if (length == 0 || k >= length)
        return 0;

    return quickSelect(array, 0, length - 1, k, compare, swap);
}