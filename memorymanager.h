#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

#include "ram.h"
#include "pcb.h"

#define PAGESIZE 4

FILE* copyFile(FILE *fp1);
int countTotalPages(FILE *fp);
FILE* findPage(int pageNumber, FILE* f);
int isSameFilePointer(FILE*f1, FILE* f2);
int findFrame(FILE* page);
int findVictim(PCB*p);
int updateFrame(int frameNumber, int victimFrame, FILE *page); // not used
int updatePageTable(PCB *p, int pageNumber, int frameNumber);
int launcher(FILE *p);
void wipeBackingStore();

#endif