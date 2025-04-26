
#include <stddef.h>

#include "BP.h"
#include "Disk.h"
#include "File.h"
#include "LinkedList.h"
#include "BST_firstfit.h" // Special BST for First-Fit

/*
    Implementation of the First-Fit strategy
*/

static int compareFileSizeDescending(const void *f1, const void *f2)
{
    return compareFileSize(f2, f1);
}

static void dummyDestroy(void *data) 
{
    (void)data;
}

static int compareDiskPointer(const void *d1, const void *d2)
{
    if (d1 < d2) return -1;
    if (d1 > d2) return 1;
    return 0;
}

size_t binpacking(size_t diskSize, List *files, List *disks)
{
    // Step 1: Sort files descending by size
    llSort(files, compareFileSizeDescending);

    // Step 2: Create an empty AVL tree
    avltree *tree = avl_create(compareDiskPointer, dummyDestroy);

    size_t nbDisks = 0;
    Node *p = llHead(files);

    while (p != NULL)
    {
        File *f = llData(p);

        // Step 3: Search for the first disk that fits
        avlnode *node = bst_firstfit_find(tree, fileSize(f));

        if (node != NULL)
        {
            // Found a disk -> add file
            diskAddFile(node->data, f);
            node->space -= fileSize(f); // Update the node's free space
            avl_update_maxspace(tree, node); // Propagate maxspace update upwards
        }
        else
        {
            // No fitting disk -> create a new disk
            Disk *newDisk = diskCreate(diskSize);
            diskAddFile(newDisk, f);

            // Insert into tree with initial free space
            avl_insert(tree, newDisk, diskFreeSpace(newDisk));

            // Add to list of disks
            llInsertLast(disks, newDisk);
            nbDisks++;
        }

        p = llNext(p);
    }

    // Step 4: Clean up
    avl_destroy(tree);

    return nbDisks;
}
