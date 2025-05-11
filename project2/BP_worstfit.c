
#include <stddef.h>

#include "BP.h"
#include "Disk.h"
#include "File.h"
#include "LinkedList.h"
#include "PQ.h" // include priority queue

/*
    Implementation of the Worst-Fit strategy
*/

static int compareFileSizeDescending(const void *f1, const void *f2)
{
    return compareFileSize(f2, f1);  // Flip order for descending sort
}

size_t binpacking(size_t diskSize, List *files, List *disks)
{
    // Sort files in descending order of size before packing
    llSort(files, compareFileSizeDescending);

    // Create a priority queue with enough capacity
    PQ *pq = pqCreate(llLength(files), compareDiskFreeSpace);

    Node *p = llHead(files);
    size_t nbDisks = 0;

    while (p != NULL)
    {
        File *f = llData(p);
        Disk *targetDisk = NULL;

        // Try to use the disk with maximum free space
        if (pqSize(pq) > 0)
        {
            Disk *maxFreeDisk = pqExtractMax(pq);

            if (diskAddFile(maxFreeDisk, f))
            {
                targetDisk = maxFreeDisk;
            }
            else
            {
                // Disk too small, reinsert it unchanged
                pqInsert(pq, maxFreeDisk);
            }
        }
        
        // If no suitable disk found, create a new one
        if (targetDisk == NULL)
        {
            targetDisk = diskCreate(diskSize);
            nbDisks++;
            diskAddFile(targetDisk, f);

            llInsertLast(disks, targetDisk);
        }

        pqInsert(pq, targetDisk); // Reinsert into PQ but with updated free space
        p = llNext(p);
    }

    pqFree(pq);
    return nbDisks;
}