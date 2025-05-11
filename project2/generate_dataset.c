#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // for mkdir
#include <time.h>

#include "File.h"
#include "LinkedList.h"

static List *generateRandomFiles(size_t diskSize, size_t nbFiles)
{
    List *files = llCreateEmpty();
    char buffer[1024];
    for (size_t i = 0; i < nbFiles; i++)
    {
        sprintf(buffer, "f%zu", i);
        File *f = fileCreate(strdup(buffer), rand() % diskSize + 1);
        llInsertLast(files, f);
    }
    return files;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <disk_size> <number_of_files> <output_name.csv>\n", argv[0]);
        return 1;
    }

    size_t diskSize = strtoull(argv[1], NULL, 10);
    size_t nbFiles = strtoull(argv[2], NULL, 10);
    char *outputName = argv[3];

    srand(time(NULL));

    // Create /datasets folder if not exists
    struct stat st = {0};
    if (stat("datasets", &st) == -1)
    {
        mkdir("datasets", 0755);
    }

    // Compose full output path
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "datasets/%s", outputName);

    // Open the file to write
    FILE *outfile = fopen(fullPath, "w");
    if (!outfile)
    {
        fprintf(stderr, "Cannot open output file %s\n", fullPath);
        return 1;
    }

    // Generate random files
    List *files = generateRandomFiles(diskSize, nbFiles);

    // Write files to CSV
    Node *p = llHead(files);
    while (p)
    {
        File *f = llData(p);
        fprintf(outfile, "%s,%zu\n", fileName(f), fileSize(f));
        p = llNext(p);
    }

    fclose(outfile);
    llFreeData(files, fileFree);

    printf("Dataset with %zu files generated at %s\n", nbFiles, fullPath);

    return 0;
}

// gcc -std=c99 --pedantic -Wall -Wextra -Wmissing-prototypes -o generate_dataset generate_dataset.c File.c LinkedList.c
