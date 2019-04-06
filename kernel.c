#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "shell.h"
#include "shellmemory.h"
#include "cpu.h"
#include "pcb.h"
#include "kernel.h"
#include "ram.h"


ReadyQueueNode* READYHEAD;


void addToReady(PCB* toAddPCB)
{
	ReadyQueueNode* current =  READYHEAD;
	ReadyQueueNode* previous = READYHEAD;
	int current_id=0;
	while (current != (ReadyQueueNode*)NULL)
	{
		previous = current;
		current = current->next;
		current_id++;
	}

	ReadyQueueNode * newnode = malloc(sizeof(ReadyQueueNode));
	newnode->pcb = toAddPCB;
	newnode->next= NULL;
	newnode->id = current_id;

	current = newnode;

	if (current_id == 0)
	{
		READYHEAD = current;
	}
	else
	{
		previous->next = current;
	}
	return;
}

PCB* dequeueFromReady()
{
	ReadyQueueNode* headNext = READYHEAD->next;
	ReadyQueueNode* result = READYHEAD;
	READYHEAD = headNext; 
	return result->pcb;
}

int scheduler()
{	
	int status = -1;
	PCB* nextItem;
	if (cpu->IP == (FILE* )NULL)
	{
		nextItem = dequeueFromReady();
		status = runPCB(nextItem);
	}

	if( status == 1) // the PCB has computed everything
	{
		fseek(nextItem->PC, 0, SEEK_SET); 
		freeFromRAM(nextItem); // IS THIS CORRECT? 
		free(nextItem); 
	}
	else  	// the PCB have some left over  == 0
	{	
		addToReady(nextItem);
	}

	return status;
}


void boot()
{
	CPUInitialize();
	RAMInitialize();
	
	struct stat st = {0};
	if (stat("./BackingStore", &st) == -1) {
    	mkdir("./BackingStore/",  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    	printf("heeere");
	}else{
		system("exec rm -rf ./BackingStore/");
		mkdir("./BackingStore/",  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

}

int main()
{
	boot();
	UIInstantiate();
}