#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ram.h"
#include "kernel.h"

#define RAMSPACE 10

FILE *ram[RAMSPACE];

void RAMInitialize()
{
	int i =0; 
	while (i< RAMSPACE)
	{
		ram[i] = (FILE*) (NULL);
		i++;
	}

}


int addToRAM(FILE *fp)
{
	// return : the index of ram at which fp is added
	char **args;
	char str[MAXCHAR];

	//finding next availabe pointer
	int result=-1;
	int index;
	for (index = 0; index < RAMSPACE; index++)
	{
		if (ram[index] == (FILE*) NULL)
		{
			ram[index] = fp; //assign pointer to empty space 
			result=index;
			break;
		} 
	}
	return result;
}


void freeFromRAM(PCB *pcb)
{	/* Delete all pages associate with the pcb from ram
		*/
    int p_i;
    for (p_i = 0; p_i < RAMSPACE; p_i++)
    {
    	int frameNumber=pcb->pageTable[p_i];
        if (frameNumber!= -1) // if it has a valid entry
      	{
        	fclose(ram[frameNumber]);
        	ram[frameNumber] = NULL; 
      	}
    }
    //printf("process cleaned from ram!\n");
    //printRAM();
}


void wipeRAM()
{
    int index;
    for (index = 0; index < RAMSPACE; index++)
    {
    	fclose(ram[index]);
        ram[index] =NULL;
    }
}


void printRAM()
{
	int i;
	printf("===== RAM =========\n");
	for (i =0; i<10; i++)
	{
		printf("%d slot is: %p\n",i,(ram[i]));

	}
		printf("==================\n");

}
