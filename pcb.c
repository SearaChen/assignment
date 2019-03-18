#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "pcb.h"


PCB* makePCB(FILE* fp)
{
	PCB* result = NULL;
	result = malloc(sizeof(PCB)); //TODO: missing a free 
	result-> PC = fp;
	return result;
}
