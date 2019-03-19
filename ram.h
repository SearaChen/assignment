#ifndef _RAM_H_
#define _RAM_H_
 
#define RAMSIZE 10 
extern FILE *ram[]; // TO TAKE OUT AFTER DEBUG
void RAMInitialize();
int addToRAM(FILE *fp);
void freeFromRAM(FILE *fp);
#endif