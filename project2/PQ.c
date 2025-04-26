
#include "PQ.h"

// Placez ici votre implémentation de la file à priorité

#include <stdlib.h>

struct PQ_t {
    void **data;
    size_t capacity;
    size_t size;
    int (*compare)(const void *, const void *);
};

// Swap two elements in the heap
static void swap(void **a, void **b) {
    void *temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify down from index i to maintain max-heap property
static void maxHeapify(PQ *pq, size_t i) {
    size_t largest = i;
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;

    if (left < pq->size && pq->compare(pq->data[left], pq->data[largest]) > 0)
        largest = left;

    if (right < pq->size && pq->compare(pq->data[right], pq->data[largest]) > 0)
        largest = right;

    if (largest != i) {
        swap(&pq->data[i], &pq->data[largest]);
        maxHeapify(pq, largest);
    }
}

// Create a new priority queue with given capacity and comparison function
PQ *pqCreate(size_t capacity, int (*compare)(const void *, const void *)) {
    PQ *pq = malloc(sizeof(PQ));
    if (!pq) return NULL;

    pq->data = malloc(sizeof(void *) * capacity);
    if (!pq->data) {
        free(pq);
        return NULL;
    }

    pq->capacity = capacity;
    pq->size = 0;
    pq->compare = compare;
    return pq;
}

// Free memory used by the priority queue
void pqFree(PQ *pq) {
    if (!pq) return;
    free(pq->data);
    free(pq);
}

// Insert a new element and maintain max-heap property (heapify-up)
bool pqInsert(PQ *pq, void *key) {
    if (!pq || pq->size >= pq->capacity) return false;

    size_t i = pq->size;
    pq->data[i] = key;

    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (pq->compare(pq->data[i], pq->data[parent]) > 0) {
            swap(&pq->data[i], &pq->data[parent]);
            i = parent;
        } else {
            break;
        }
    }

    pq->size++;
    return true;
}


// Return the max element without removing it
void *pqGetMax(const PQ *pq) {
    if (pq->size == 0) return NULL;
    return pq->data[0];
}

// Remove and return the max element, then heapify-down
void *pqExtractMax(PQ *pq) {
    if (pq->size == 0) return NULL;

    void *max = pq->data[0];
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;
    maxHeapify(pq, 0);
    return max;
}

// Return current number of elements
size_t pqSize(const PQ *pq) {
    return pq->size;
}

// Return the maximum capacity of the queue
size_t pqCapacity(const PQ *pq) {
    return pq->capacity;
}
