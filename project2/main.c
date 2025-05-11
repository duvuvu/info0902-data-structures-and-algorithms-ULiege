/*
 * Updated main.c for empirical analysis: export rich results to CSV file
 */

 #define _POSIX_C_SOURCE 200809L

 #include <stddef.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 #include <time.h>
 #include <math.h>
 
 #include "Disk.h"
 #include "File.h"
 #include "BP.h"
 
 static List *readFiles(char *filename)
 {
     FILE *csvfile = fopen(filename, "r");
     if (!csvfile)
     {
         fprintf(stderr, "Could not open file %s\n", filename);
         return NULL;
     }
 
     List *files = llCreateEmpty();
     char line[1024];
     while (fgets(line, 1024, csvfile))
     {
         if (strlen(line) <= 1)
             continue;
         char *name = strdup(strtok(line, ","));
         size_t size = strtoull(strtok(NULL, ",\n"), NULL, 10);
         File *file = fileCreate(name, size);
         llInsertLast(files, file);
     }
     fclose(csvfile);
     return files;
 }
 
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
 
 // New: compute total file size
 static size_t totalFileSize(List *files)
 {
     size_t totalsize = 0;
     Node *p = llHead(files);
     while (p)
     {
         totalsize += fileSize(llData(p));
         p = llNext(p);
     }
     return totalsize;
 }
 
 // New: collect rich results and write to CSV
 static void writeResult(FILE *csv, size_t nFiles, size_t totalSize, size_t diskSize,
                         size_t minDisks, size_t maxDisks, size_t disksUsed, size_t waste,
                         double cpuTime)
 {
     fprintf(csv, "%zu,%zu,%zu,%zu,%zu,%zu,%zu,%.6f\n",
             nFiles, totalSize, diskSize, minDisks, maxDisks, disksUsed, waste, cpuTime);
 }
 
 int main(int argc, char *argv[])
 {
     srand(42); // Reproducibility
 
     if (argc != 4)
     {
         fprintf(stderr, "Usage: %s <disk_size> <output_csv_file> <list_of_files>\n", argv[0]);
         fprintf(stderr, "Example: %s 1000000 results.csv datasets/list.txt\n", argv[0]);
         return 1;
     }
 
     size_t diskSize = strtoull(argv[1], NULL, 10);
     const char *csvOutput = argv[2];
     const char *listFile = argv[3];
 
     FILE *list = fopen(listFile, "r");
     if (!list)
     {
         fprintf(stderr, "Cannot open list of files: %s\n", listFile);
         return 1;
     }
 
     FILE *csv = fopen(csvOutput, "w");
     if (!csv)
     {
         fprintf(stderr, "Cannot open output CSV file: %s\n", csvOutput);
         fclose(list);
         return 1;
     }
 
     fprintf(csv, "N,TotalFileSize,DiskSize,MinDisks,MaxDisks,DisksUsed,Waste,CPUTime\n"); // Header
 
     char filename[1024];
     while (fgets(filename, sizeof(filename), list))
     {
         filename[strcspn(filename, "\r\n")] = 0; // Remove newline
         if (strlen(filename) == 0)
             continue;
 
         List *files = readFiles(filename);
         if (!files)
         {
             fprintf(stderr, "Skipping file %s\n", filename);
             continue;
         }
 
         List *disks = llCreateEmpty();
 
         clock_t start = clock();
         binpacking(diskSize, files, disks);
         clock_t end = clock();
 
         double cpuTime = (double)(end - start) / (double)CLOCKS_PER_SEC;
         size_t totalSize = totalFileSize(files);
         size_t nFiles = llLength(files);
         size_t minDisks = (size_t)ceill((double)totalSize / (double)diskSize);
         size_t maxDisks = nFiles;
         size_t disksUsed = llLength(disks);
         size_t waste = disksUsed * diskSize - totalSize;
 
         writeResult(csv, nFiles, totalSize, diskSize, minDisks, maxDisks, disksUsed, waste, cpuTime);
 
         llFreeData(disks, diskFree);
         llFreeData(files, fileFree);
     }
 
     fclose(csv);
     fclose(list);
 
     printf("Done! Results written to %s\n", csvOutput);
 
     return 0;
 }
 