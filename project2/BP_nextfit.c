
#include <stddef.h>

#include "BP.h"
#include "Disk.h"
#include "File.h"
#include "LinkedList.h"

/*
    Implementation of the Next-Fit strategy
*/

static int compareFileSizeDescending(const void *f1, const void *f2)
{
    return compareFileSize(f2, f1);  // Flip order for descending sort
}

size_t binpacking(size_t diskSize, List *files, List *disks)
{
    // Sort files in descending order of size before packing
    llSort(files, compareFileSizeDescending);

    Node *p = llHead(files);
    size_t nbDisks = 0;

    Disk *currentDisk = NULL;

    while (p != NULL)
    {
        File *f = llData(p);

        // If no disk exists or file doesn't fit, create a new disk
        if (currentDisk == NULL || !diskAddFile(currentDisk, f))
        {
            currentDisk = diskCreate(diskSize);
            diskAddFile(currentDisk, f);
            llInsertLast(disks, currentDisk);
            nbDisks++;
        }

        p = llNext(p);
    }

    return nbDisks;
}
