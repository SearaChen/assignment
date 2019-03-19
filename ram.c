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