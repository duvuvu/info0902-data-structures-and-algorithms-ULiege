
#include <stddef.h>

#include "BP.h"
#include "Disk.h"
#include "File.h"
#include "LinkedList.h"
#include "BST_bestfit.h"  // Use our new Best-Fit specialized BST

/*
    Implementation of the Best-Fit strategy
*/

static int compareFileSizeDescending(const void *f1, const void *f2)
{
    return compareFileSize(f2, f1);
}

static void dummyDestroy(void *data)
{
    (void)data;
}

size_t binpacking(size_t diskSize, List *files, List *disks)
{
    // Sort files in descending order of size before packing
    llSort(files, compareFileSizeDescending);

    // Create an empty AVL tree
    avltree *tree = avl_create(compareDiskFreeSpace, dummyDestroy);

    size_t nbDisks = 0;
    Node *p = llHead(files);

    while (p != NULL)
    {
        File *f = llData(p);

        // Step 3: Search for the best-fit disk
        avlnode *node = bst_find_bestfit(tree, fileSize(f));

        if (node != NULL)
        {
            // Found a disk -> remove it temporarily
            avl_delete(tree, node, 1); // 1 = keep the disk data

            // Add file to the disk
            diskAddFile(node->data, f);

            // Reinsert the updated disk into the tree
            avl_insert(tree, node->data);
        }
        else
        {
            // No fitting disk -> create a new disk
            Disk *newDisk = diskCreate(diskSize);
            diskAddFile(newDisk, f);

            // Insert new disk into the tree
            avl_insert(tree, newDisk);

            // Add new disk to linked list
            llInsertLast(disks, newDisk);
            nbDisks++;
        }

        p = llNext(p);
    }

    // Step 4: Clean up
    avl_destroy(tree);

    return nbDisks;
}
