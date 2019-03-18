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

void addToRAM(FILE *fp)
{
	char **args;
	char str[MAXCHAR];

	//finding next availabe pointer
	int index;
	for (index = 0; index < RAMSPACE; index++)
	{
		if (ram[index] == (FILE*) NULL)
		{
			ram[index] = fp; //assign pointer to empty space 
			break;
		} 
	}
}

void freeFromRAM(FILE *fp)
{
    int index;
    for (index = 0; index < RAMSPACE; index++)
    {
        if (ram[index] == fp)
        {
            //assign pointer to empty space 
            fclose(ram[index]);
            ram[index] = NULL; 
            break;
        } 
    }
}