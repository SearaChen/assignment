#ifndef _KERNEL_H_
#define _KERNEL_H_

#define MAXCHAR 1000

#include "pcb.h"

typedef struct readyqueuenode{
	int id;
	PCB* pcb;
	struct readyqueuenode * next;
} ReadyQueueNode;

extern ReadyQueueNode* READYHEAD;

void myinit(FILE* fp);
int scheduler();


#endif 
