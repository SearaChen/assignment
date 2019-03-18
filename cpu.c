#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "shellmemory.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"

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
	cpu->IP=currentPCB->PC;

	int lineCount = 0;
	char **args;

	//printf("before next File position :  %ld\n", ftell(cpu->IP));
	while (lineCount < cpu->quanta && fgets(cpu->IR, MAXCHAR, cpu->IP) != NULL ) // if switch order will induce +1 file pointer error
	{
		//printf("The current instruction is: %s",cpu->IR);
		//printf("current FILE position: %ld\n", ftell(cpu->IP));
		args = parse(cpu->IR);

		execute(args);
		lineCount ++;
		//printf("before next FILE position: %ld\n", ftell(cpu->IP));
	}
	//printf("AFTER QUANTA FILE position: %ld\n", ftell(cpu->IP));
	currentPCB->PC = cpu->IP;
	cpu->IP= NULL;

	if (lineCount < cpu->quanta) // if finished before
	{
		return 1;
	}
	return 0;

}