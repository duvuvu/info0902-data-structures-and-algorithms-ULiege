/* ========================================================================= *
 * HeapSelect
 * Implementation of the HeapSelect algorithm
 * ========================================================================= */

 #include <stddef.h>
 #include "Select.h"

// Min heapify function
static void minHeapify(void *array, size_t heapSize, size_t i,
                       int (*compare)(const void *, size_t, size_t),
                       void (*swap)(void *, size_t, size_t))
{
    size_t smallest = i; // initialize smallest as root of the subtree
    size_t l = 2*i + 1; // the left child of the i-th parent node
    size_t r = 2*i + 2; // the right child of the i-th parent node

    // If left child exists and is smaller than root
    if (l < heapSize && compare(array, l, smallest) < 0)
        smallest = l;

    // If right child exists and is smaller than the current smallest
    if (r < heapSize && compare(array, r, smallest) < 0)
        smallest = r;

    // If the smallest is not the root, swap then continue heapifying
    if (smallest != i) {
        swap(array, i, smallest);
        minHeapify(array, heapSize, smallest, compare, swap);
    }
}

// Build min heap function
static void buildMinHeap(void *array, size_t length,
                         int (*compare)(const void *, size_t, size_t),
                         void (*swap)(void *, size_t, size_t))
{
    for (size_t i = length / 2; i > 0; i--)
        minHeapify(array, length, i - 1, compare, swap);
}

// Heap Select function
size_t select(void *array, size_t length, size_t k,
              int (*compare)(const void *, size_t i, size_t j),
              void (*swap)(void *array, size_t i, size_t j))
{
    if (length == 0 || k >= length)
        return 0;

    buildMinHeap(array, length, compare, swap);

    size_t heapSize = length;
    for (size_t i = 0; i <= k; i++) {
        swap(array, 0, heapSize - 1); 
        heapSize--; // Reduce heap size
        minHeapify(array, heapSize, 0, compare, swap);
    }
    
    return length - k - 1;
}