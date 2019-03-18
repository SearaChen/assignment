#ifndef _CPU_H_
#define _CPU_H_

#include "kernel.h"

typedef struct { 
	FILE *IP; 
	char IR[MAXCHAR]; 
	int quanta; 
}CPU; 

extern CPU* cpu;
void CPUInitialize();
int runPCB(PCB* currentPCB);


#endif