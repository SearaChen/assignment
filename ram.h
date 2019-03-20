#ifndef _RAM_H_
#define _RAM_H_

#include "pcb.h" 

#define RAMSIZE 10 

extern FILE *ram[]; // TO TAKE OUT AFTER DEBUG
void RAMInitialize();
int addToRAM(FILE *fp);
void freeFromRAM(PCB *fp);
void wipeRAM();
void printRAM();

#endif