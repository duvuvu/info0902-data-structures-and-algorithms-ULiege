#include <stddef.h>

#include "BP.h"
#include "Disk.h"
#include "File.h"
#include "LinkedList.h"
#include "BST_bestfit.h" // Use the best-Fit BST

/*
    Implementation of the Best-Fit strategy
*/

static int compareFileSizeDescending(const void *f1, const void *f2)
{
    return compareFileSize(f2, f1); // flip order for descending sort
}

static void dummyDestroy(void *data) 
{
    (void)data;
}

static int compareDiskFreeSpaceStrict(const void *d1, const void *d2)
{
    size_t free1 = diskFreeSpace((Disk *)d1);
    size_t free2 = diskFreeSpace((Disk *)d2);

    if (free1 != free2)
        return (free1 < free2) ? -1 : 1;

    // Break ties by comparing pointer addresses
    return (d1 < d2) ? -1 : (d1 > d2);
}

size_t binpacking(size_t diskSize, List *files, List *disks)
{
    // Sort files in descending order of size before packing
    llSort(files, compareFileSizeDescending);

    // Create AVL tree ordered by disk's free space
    avltree *tree = avl_create(compareDiskFreeSpaceStrict, dummyDestroy);

    Node *p = llHead(files);
    size_t nbDisks = 0;

    while (p != NULL)
    {
        File *f = llData(p);
        avlnode *bestNode = tree_search_bf(tree, fileSize(f)); // search for best fitting disk
        Disk *targetDisk = NULL;

        if (bestNode != NULL)
        {
            // Found the best fitting disk
            targetDisk = (Disk *)bestNode->data;
            avl_delete(tree, bestNode, 0); // remove old node

            diskAddFile(targetDisk, f); // add file to disk

            avlnode *newNode = avl_insert(tree, targetDisk);
            newNode->space = diskFreeSpace(targetDisk);
        }
        else
        {
            // No fitting disk, create a new one
            targetDisk = diskCreate(diskSize);
            diskAddFile(targetDisk, f);

            llInsertLast(disks, targetDisk);
            nbDisks++;

            avlnode *newNode = avl_insert(tree, targetDisk);
            newNode->space = diskFreeSpace(targetDisk);
        }

        p = llNext(p);
    }

    // Cleanup
    avl_destroy(tree);

    return nbDisks;
}