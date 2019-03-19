#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "shellmemory.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"
#include "memorymanager.h"

#define QUANTA 2


CPU* cpu; 

void CPUInitialize()
{
	cpu = malloc(sizeof(CPU));
	cpu->quanta = QUANTA;
	cpu->IP= NULL;
}

int runPCB(PCB* currentPCB)
{
	/* return 0: if had a normal run, some left ; 1: this PCB finished executing
	*/
	printf("------------------ new quanta -------------------\n");
	printf("current page:%d\n",currentPCB->PC_page);
	printf("current offset:%d\n",currentPCB->PC_offset);
	printf("max page:%d\n", currentPCB->pages_max);
	//printf("current pcb IP:%ld\n",ftell(currentPCB->PC));

	// ------- PAGE FAULT HERE --------- (if prevention do page fault, other program might overwrite)
	//check if current frame valid
	int currentIPFrameNumber = (currentPCB->pageTable[currentPCB->PC_page]);
	printf("currentIPFrameNumber: %d\n", currentIPFrameNumber );
	FILE* currentPagePointer = findPage(currentPCB->PC_page, currentPCB->PC);

	if (currentIPFrameNumber == -1 ) // IF it is not valid or has been overwritten
	{
		int frameNumber = -1;

		// find a frame number that is empty first 
		int i;

		for (i=0;i<10;i++)
		{
			if (ram[i] == NULL)
			{
				frameNumber=i;
				break;
			}
		}
		if (frameNumber == -1) // if didnt find empty space
		{
			frameNumber = findVictim(currentPCB);
		}

		// insert in correct page
		ram[frameNumber] = currentPagePointer;
		updatePageTable(currentPCB, currentPCB->PC_page, frameNumber);
	}
	

	//load the instruction
	cpu->IP=currentPCB->PC;

	int lineCount = 0;
	char **args;

	// while(fgets(cpu->IR, 10000, cpu->IP) != NULL)
	// {
	// 	printf("instruction on cpu: %s\n",cpu->IR);
	// }
	// printf("getting insturction is null?");
	// exit(EXIT_SUCCESS);
	// if (fgets(cpu->IR, MAXCHAR, cpu->IP) == NULL)
	// 	{printf("reading failed\n");}
	// else{printf("instruction on cpu: %s\n",cpu->IR);}

	while (lineCount < cpu->quanta && fgets(cpu->IR, MAXCHAR, cpu->IP) != NULL ) // if switch order will induce +1 file pointer error
	{
		printf("instruction on cpu: %s",cpu->IR);
		args = parse(cpu->IR);
		execute(args);
		lineCount ++;
	}
	currentPCB->PC = cpu->IP;
	cpu->IP= NULL;

	//-----update PCB-----
	currentPCB->PC_offset = currentPCB->PC_offset + lineCount;
	if(lineCount< cpu->quanta)
	{
		return 1;
	}
	else if(currentPCB->PC_offset == 4) //next page needed
	{
		currentPCB->PC_offset = 0; 
		currentPCB->PC_page++; 

		if (currentPCB->PC_page +1 > currentPCB->pages_max) //over
		{
			return 1;
		}
	}

	return 0;

}