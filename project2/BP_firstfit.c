
#include <stddef.h>

#include "BP.h"
#include "Disk.h"
#include "File.h"
#include "LinkedList.h"
#include "BST_firstfit.h" // use First-Fit BST

/*
    Implementation of the First-Fit strategy
*/

static int compareFileSizeDescending(const void *f1, const void *f2)
{
    return compareFileSize(f2, f1); // flip order for descending sort
}

static void dummyDestroy(void *data) 
{
    (void)data;
}

static int compareKey(const void *a, const void *b)
{
    const avlnode *node1 = (const avlnode *)a;
    const avlnode *node2 = (const avlnode *)b;

    if (node1->key < node2->key) return -1;
    if (node1->key > node2->key) return 1;
    return 0;
}


size_t binpacking(size_t diskSize, List *files, List *disks)
{
    // Sort files in descending order of size before packing
    llSort(files, compareFileSizeDescending);

    // Create an empty AVL tree based on disk key
    avltree *tree = avl_create(compareKey, dummyDestroy);

    size_t nbDisks = 0;
    Node *p = llHead(files);

    while (p != NULL)
    {
        File *f = llData(p);
        avlnode *node = tree_search_ff(tree, fileSize(f)); // search for the first disk that fits

        if (node != NULL)
        {
            // Found the first fitting disk, add file to it
            diskAddFile(node->data, f); // add file
            node->space -= fileSize(f); // update the node's free space
            avl_update_maxspace(tree, node); // propagate maxspace update upwards
        }
        else
        {
            // No fitting disk, create a new one
            Disk *newDisk = diskCreate(diskSize);
            diskAddFile(newDisk, f);

            // Insert into tree with initial free space
            avl_insert(tree, newDisk, diskFreeSpace(newDisk), nbDisks);

            // Add to list of disks
            llInsertLast(disks, newDisk);
            nbDisks++;
        }

        p = llNext(p);
    }

    // Clean up
    avl_destroy(tree);

    return nbDisks;
}
