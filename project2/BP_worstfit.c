
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

    Node *p = llHead(files);
    size_t nbDisks = 0;

    // Create a priority queue with enough capacity
    PQ *pq = pqCreate(llLength(files), compareDiskFreeSpace);

    while (p != NULL)
    {
        // // Print all available disks and their files and used space and free space on only 1 line
        // printf("Available disks:\n");
        // Node *diskNode = llHead(disks);
        // size_t i = 0;
        // while (diskNode != NULL)
        // {
        //     i++;
        //     Disk *d = llData(diskNode);
        //     printf("Disk %zu: used space: %zu, free space: %zu, files: ", i, diskUsedSpace(d), diskFreeSpace(d));
        //     Node *fileNode = llHead(diskFiles(d));
        //     while (fileNode != NULL)
        //     {
        //         File *file = llData(fileNode);
        //         printf("%s ", fileName(file));
        //         fileNode = llNext(fileNode);
        //     }
        //     printf("\n");
        //     diskNode = llNext(diskNode);
        // }

        File *f = llData(p);
        Disk *targetDisk = NULL;

        if (pqSize(pq) > 0)
        {
            Disk *maxFreeDisk = pqExtractMax(pq);

            if (diskAddFile(maxFreeDisk, f))
            {
                targetDisk = maxFreeDisk;
            }
            else
            {
                pqInsert(pq, maxFreeDisk); // Put it back in queue
            }
        }
        
        if (targetDisk == NULL)
        {
            targetDisk = diskCreate(diskSize);
            nbDisks++;
            diskAddFile(targetDisk, f);

            llInsertLast(disks, targetDisk);
        }
        // // Print the file (with its size) added to the disk (with its updated used space and free space)
        // printf("Added file %s (size: %zu) to disk %zu (used space: %zu, free space: %zu)\n\n", fileName(f), fileSize(f), nbDisks - 1, diskUsedSpace(targetDisk), diskFreeSpace(targetDisk));

        pqInsert(pq, targetDisk); // Re-insert into PQ
        p = llNext(p);
    }

    pqFree(pq);
    return nbDisks;
}