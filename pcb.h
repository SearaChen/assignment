#ifndef _pcb_H_
#define _pcb_H_
 
typedef struct{
   FILE *PC;
   int pageTable[10];
   int PC_page;
   int PC_offset;
   int pages_max;
}PCB;

PCB* makePCB(FILE*fp);
void printPCBPageTable(PCB* pcb);

#endif