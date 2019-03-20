#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "shellmemory.h"
#include "kernel.h"
#include "pcb.h"
#include "cpu.h"
#include "shell.h"
#include "memorymanager.h"
#include "ram.h"

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
	
	// printf("------------------ new quanta -------------------\n");
	// printf("current page:%d\n",currentPCB->PC_page);
	// printf("current offset:%d\n",currentPCB->PC_offset);
	// printf("max page:%d\n", currentPCB->pages_max);

	//printRAM(); 

	// --------------PAGE FAULT HERE ----------------------------
	//check if current frame valid
	int currentIPFrameNumber = (currentPCB->pageTable[currentPCB->PC_page]);
	//printf("currentIPFrameNumber: %d\n", currentIPFrameNumber );
	//printPCBPageTable(currentPCB);
	
	FILE* currentPagePointer = findPage(currentPCB->PC_page, currentPCB->PC);

	if (currentIPFrameNumber == -1 ) // IF it is not valid or has been overwritten
	{
		//printf("!!!!!!!!! PAGE FAULT!!!!!!!!!!!!!\n");
		int frameNumber = findFrame(currentPagePointer);

		if (frameNumber == -1) // if didnt find empty space
		{
			frameNumber = findVictim(currentPCB);
			//printf("did not find empty frame to use, select victim frame: %d\n",frameNumber);

			ram[frameNumber] = currentPagePointer;

			// update victim page table
			ReadyQueueNode* current = READYHEAD;
			int foundVictimPCB =0;
			while (current != (ReadyQueueNode*)NULL)
			{
				//check for everypage in a process
				int p_num;
				for (p_num =0; p_num< 10;p_num++)
				{
					if(current->pcb->pageTable[p_num]==frameNumber) // found belonging
					{
						current->pcb->pageTable[p_num]=-1;
						foundVictimPCB=1;
						break;
					}
				}
				if(foundVictimPCB ==1)
				{
					break;
				}
				current = current->next;


			}

			if (foundVictimPCB==0)
			{
				//printf("Error! Victim Frame PCB not found!\n");
				exit(EXIT_SUCCESS);
			}

			// update this page table 
			currentPCB->pageTable[currentPCB->PC_page] = frameNumber;
			//printPCBPageTable(currentPCB);
		}
		else
		{
			ram[frameNumber] = currentPagePointer;

			// update this page table 
			currentPCB->pageTable[currentPCB->PC_page] = frameNumber;			
			//printf("found empty frame to use: %d\n",frameNumber);
		}
	}

	// printRAM();
	// printPCBPageTable(currentPCB);
	

	//load the instruction, note: not ACTUALLY dependent on ram
	cpu->IP=currentPCB->PC;

	int lineCount = 0;
	char **args;

	while (lineCount < cpu->quanta && fgets(cpu->IR, MAXCHAR, cpu->IP) != NULL ) // if switch order will induce +1 file pointer error
	{
		//printf("instruction on cpu: %s\n",cpu->IR);
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