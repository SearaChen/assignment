#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "DISK_driver.h"

// global data structures definition 
typedef struct PARTIT {
    int total_blocks;
    int block_size;
} Partition;


typedef struct FAT {
    char *filename;
    int file_length;
    int blockPtrs[10];
    int current_location;
} FileIndexObject;


//global data structure instantiation 
FileIndexObject* fat[20];
char * block_buffer;
FILE *fp[5];
int fpInfo[5]; // stores the index of its corresding FileIndexObject location in fat
Partition* PARTITION;

// global data structures 
void initIO()
{
	// initialize FAT
	int i;
	for(i =0; i < 20 ; i++)
	{	
		fat[i] = malloc(sizeof(FileIndexObject));
		fat[i] = (NULL);
	}

	// initialize fp
	for(i =0; i < 5 ; i++)
	{
		fp[i] = NULL;
	}

	// initialize block_buffer?? 

	//initialize partition 
	PARTITION = malloc(sizeof(Partition));


}

int partition(char *name, int blocksize, int totalblocks)
{
	// making partition directory if does not exist already 
	struct stat st = {0};
	if (stat("./PARTITION/", &st) == -1) {
    	mkdir("./PARTITION/",  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	//create partition
	PARTITION->total_blocks = totalblocks;
	PARTITION->block_size = blocksize;

	// create the file with file name
	char partitionPath[200];
	//char* partitionPath = "./PARTITION/";

	strcpy(partitionPath, "./PARTITION/");
	strcat( partitionPath, name);
	FILE *partitionP; 
	partitionP = fopen( partitionPath, "w+");

	// test area
	// make up a testobject, file it at 0,
	FileIndexObject * newFile = malloc(sizeof(FileIndexObject));
	newFile->filename = "FIRSTFILE";
	newFile->file_length = 20;
	newFile->blockPtrs[0]= 1;
	newFile->current_location = 13;
	//exit(EXIT_SUCCESS);

	fat[0] = newFile;


	//write information to file  PARTITION : FAT : BLOCKS
	fwrite(PARTITION, sizeof(Partition), 1, partitionP);

	int i = 0;
	for (i=0; i < 20; i++)
	{	
		if(fat[i] == NULL)
		{break;}
		fwrite(fat[i], sizeof(FileIndexObject), 1, partitionP);
		//exit(EXIT_SUCCESS);

	}
	//fwrite(fat, sizeof(FileIndexObject),20, partitionP);





// test areaaaaaa
	// for (i =0; i < 20; i++)
	// {
	// 	//input= fat[i];
	// 	if (fat[i] == NULL)
	// 	{
	// 		printf("It is null!\n");
	// 	}
	// 	else{
	// 		printf("filename: %s\n", fat[i]->filename);
	// 		printf("file length: %d\n", fat[i] ->file_length);

	// 	}
		
	// }
	// exit(EXIT_SUCCESS);
	// ==================

	for (i=0;i<blocksize*totalblocks; i++)  // writing list of 0 
	{
		fputc('0',partitionP);
	}
	fclose(partitionP);
	return 1;
}


int mount(char *name)
{
	// PURPOSE: decode the information of a partition, put them in global data structures 
	// Input: the partition's name

	int totalblocks = -1;
	int blocksize = -1;


	// create partition here ?? 
	PARTITION->total_blocks = totalblocks;
	PARTITION->block_size = blocksize;

	return 1;
}
int openfile(char *name)
{
	int i=-1;
	int isFound=-1;
	for (i =0; i < 20 ; i++) // looping through fat 
	{
		if(fat[i]!=NULL)
		{
			if(strcmp(fat[i]->filename, name)==0)
			{
				isFound = 0;
				break;
			}
		}
	}

	if(isFound == -1)
	{
		int j;
		for (j =0; j< 20; j++) // finding an empty spot in FAT
		{
			if(fat[j] == NULL)
			{
				isFound =0;
				i=j;
				break;
			}
		}
	}

	if (isFound == -1)
	{
		return -1; // an error occured
	}
	else
	{
		// make fp array has the file pointer to content(index i) in it
		// update corresponding spot in fpInfo with i 
	}
	return i;
}


int readBlock(int fileFATNum)
{
	FileIndexObject *current = fat[fileFATNum];
}
// char *returnBlock();
// int writeBlock(int file, char *data);

int main()
{
	initIO();
	partition("test.txt", 5, 4);

	// clear
	free(PARTITION);
	initIO();

	// try to recover
	FILE *infile;
	infile=fopen("./PARTITION/test.txt","r");
	fread(PARTITION, sizeof(Partition),1,infile);
	printf("blocksize: %d\n", PARTITION->block_size);
	printf("blocknumber: %d\n", PARTITION->total_blocks);

	FileIndexObject* input;
	input = malloc(sizeof(FileIndexObject));
	int i=0;
	/*while(fread(&fat, sizeof(FileIndexObject), 20, infile)) 
	{
		printf("hiii\n");
		//fat[i] = input;
		//i++;
		// if (fat[i] == NULL)
		// {
		// 	printf("It is null!\n");
		// 	continue;
		// }
		// printf("filename: %s\n", input->filename);
		// printf("file length: %d\n", input ->file_length);
	}*/

	while(fread(input, sizeof(FileIndexObject), 1, infile))
	{
		//input = fat[i];
		fat[i] =input;
		if (input == (FileIndexObject*)NULL)
		{break;}
		printf("filename: %s\n", fat[i]->filename);
		printf("file length: %d\n", fat[i] ->file_length);
		//free(input);
		input = malloc(sizeof(FileIndexObject));
		i++;

	}
	fclose(infile);
	//exit(EXIT_SUCCESS);

	for (i =0; i < 20; i++)
	{
		//input= fat[i];
		printf("ROUND: %d\n",i );
		if (fat[i] == NULL)
		{
			printf("%dIt is null!\n",i);
		}
		else{
			printf("filename: %s\n", fat[i]->filename);
			printf("file length: %d\n", fat[i] ->file_length);
		}	
	}
	//exit(EXIT_SUCCESS);
	return 0;
}