#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "pcb.h"
#include "memorymanager.h"


PCB* makePCB(FILE* fp)
{
	PCB* result = NULL;
	result = malloc(sizeof(PCB)); //TODO: missing a free 
	result-> PC = fp;
	int i;
	for(i=0; i<10;i++)
	{
		result->pageTable[i]=-1;
	}

	result->PC_page=0;
    result->PC_offset=0;
    //result->pages_max= countTotalPages(fp); assign it here breaks it...
	return result;
}

void printPCBPageTable(PCB* pcb)
{
	int i;
	printf("==== PCB PAGE TABLE =====\n");
	for (i=0;i<10;i++)
	{
		printf("At page index: %d -- %d\n",i,pcb->pageTable[i]);
	}
		printf("==================\n");

}

