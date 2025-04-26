
#include <stddef.h>

#include "BP.h"
#include "Disk.h"
#include "File.h"
#include "LinkedList.h"

/*
    A very suboptimal bin packing heuristic: each file is stored on a different disk.
*/

size_t binpacking(size_t diskSize, List *files, List *disks)
{
    Node *p = llHead(files); // Pointer to traverse the list of files

    size_t nbDisks = 0; // Counter to track how many disks we've created

    while (p != NULL) // Loop through all files
    {
        File *f = llData(p); // Get the current file from the node
        
        Disk *currentDisk = diskCreate(diskSize); // Create a new disk of given size
        nbDisks++; // Increment disk count
        diskAddFile(currentDisk, f); // Place the file on the newly created disk

        llInsertLast(disks, currentDisk); // Add this disk to the list of output disks

        p = llNext(p); // Move to the next file
    }

    return nbDisks; // Return the total number of disks created
}
