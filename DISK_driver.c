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
    int numOfFiles;
} Partition;


typedef struct FAT {
    char *filename;
    int file_length;	// file length in number of blocks
    int blockPtrs[10]; 
    int current_location;
} FileIndexObject;


//global data structure instantiation 
FileIndexObject* fat[20];
char * block_buffer;
FILE *fp[5];
int fpInfo[5]; // stores the index of its corresding FileIndexObject location in fat
Partition* PARTITION;
char mountPartitionName[500];

void encodeGlobalVariableToFile(FILE *partitionP)
{
	
	//write information to file  PARTITION : FAT : BLOCKS
	fwrite(PARTITION, sizeof(Partition), 1, partitionP);

	int i = 0;
	for (i=0; i < 20; i++)
	{	
		if(fat[i] == NULL)
		{continue;}
		fwrite(fat[i], sizeof(FileIndexObject), 1, partitionP);
	}
}


void decodeGlobalVariableFromFile(FILE *infile)
{
	
	// read partition
	fread(PARTITION, sizeof(Partition),1,infile);
	
	// read fat
	FileIndexObject* input;
	input = malloc(sizeof(FileIndexObject));
	int i=0;

	printf("decoding...\n" );
	while(fread(input, sizeof(FileIndexObject), 1, infile) && i<PARTITION->numOfFiles)
	{
		//input = fat[i];
		if (input == (FileIndexObject*)NULL)
		{
			continue;
		}
		fat[i] =input;

		printf("filename: %s\n", fat[i]->filename);
		printf("file length: %d\n", fat[i] ->file_length);
		//free(input);
		input = malloc(sizeof(FileIndexObject));
		i++;

	}
}

// FILE* windToStartOfPartitionData(FILE* fp2)
// { 	//PURPOSE: take the mounted file pointer, wind to the start of data section of partition file
// 	//Input : the mounted PARTITION file pointer
// 	printf("printing REWIND.....\n");
// 	fseek(fp2, 0, SEEK_SET);


// 	// skip partition
// 	Partition* temp;
// 	temp=malloc(sizeof(Partition));
// 	fread(temp, sizeof(Partition), 1, fp2);

// 	// skip sat
// 	int round = PARTITION->readToBlockNumber;
// 	fseek(fp2, (sizeof(Partition)+(round)*sizeof(FileIndexObject)), SEEK_SET);
	

//     return fp2;
// }

FILE* readBlockNumberContent(FILE* f, int i)
{	
	FILE *fpDup = fdopen (dup (fileno (f)), "r");
	int round = PARTITION->numOfFiles;

	fseek(fpDup, (sizeof(Partition)+(round)*sizeof(FileIndexObject)+ i), SEEK_SET);
	return fpDup;
}

// global data structures 
void initIO()
{
	// initialize FAT
	int i;
	for(i =0; i < 20 ; i++)
	{	
		fat[i] = (NULL);
	}

	// initialize fp
	for(i =0; i < 5 ; i++)
	{
		fp[i] = NULL;
	}

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
	PARTITION->numOfFiles = 0;

	// create the file with file name
	char partitionPath[200];
	strcpy(partitionPath, "./PARTITION/");
	strcat( partitionPath, name);
	FILE *partitionP; 
	partitionP = fopen( partitionPath, "w+");	

	// ===================================== TEST AREA =============================================
	int firstBlockPointer = 10;
	FileIndexObject * newFile = malloc(sizeof(FileIndexObject));
	newFile->filename = "BTS";
	newFile->file_length = 20;
	newFile->blockPtrs[0]= firstBlockPointer;
	newFile->current_location = 13;

	fat[0]=newFile;
	PARTITION->numOfFiles = 1;

	// =============================================================================================


	//write information to file  PARTITION : FAT : BLOCKS
	encodeGlobalVariableToFile(partitionP);
	int i;
	char c ='0';
	for (i=0;i<blocksize*totalblocks; i++)  // writing list of 0 
	{	
		//printf("%d putting characters!\n",i);

		if (i == firstBlockPointer) {
			c='s';
		}
		else
		{
			c='0';
		}

		//printf ("%d byte is: %c\n", i,c);

		int result= putc(c, partitionP) ;
		//printf ("%d result byte is: %c\n", i,result);

	}	
	fclose(partitionP);
	return 1;
}

	


FILE* mount(char *name)
{
	// PURPOSE: decode the information of a partition, put them in global data structures 
	// Input: the partition's name

	int totalblocks = -1;
	int blocksize = -1;

	//printf("THIS is the block: %d\n", parit)

	FILE * f; 
	struct stat st = {0};
	if (stat(name, &st) == -1) 
	{
		return NULL; // error code 
	}


	f= fopen(name,"r");

	decodeGlobalVariableFromFile(f);
	
 	block_buffer = malloc(sizeof(char)*PARTITION->total_blocks*PARTITION->block_size);

 	strcpy(mountPartitionName, name);

	return f;
}


int openfile(char *name)
{	// INPUT: the name of the file looking for
	//RETURN: file's FAT index , -1 when FAT/fp is full
	// Note: if name does not exist, file with "name" is created, no pointer assign to fp
	//		 if name exit, FILE pointer to the first block assignmed to fp

	int i=-1; // FAT index 
	int isFound=-1;

	for (i =0; i < 20 ; i++) // looping through fat 
	{
		if(fat[i]!= NULL && fat[i]!= (FileIndexObject*) NULL)
		{

			//printf("Current filename: %s\n",fat[i]->filename);
			if(strcmp(fat[i]->filename, name)==0)
			{
				isFound = 0;
				break;
			}
		}
	}
	printf("is found? %d\n",isFound );


	if (isFound == 0) //found in existing FAT 
	{
		int firstBlockPointer = fat[i] -> blockPtrs[0];
		
		// find an available spot in fp
		int fpIndex = -1;
		for(fpIndex=0; fpIndex<5;fpIndex++)
		{
			if (fp[fpIndex] == NULL)
			{
				break;
			}
		}

		if (fpIndex == 5){ // did not find available spot for fpIndex 
			return -1; //error
		}else{
			FILE * mountedfp;
			printf("first block pointer: %d\n", firstBlockPointer );
			mountedfp= fopen(mountPartitionName,"r");
			FILE* blockFilePtr = readBlockNumberContent(mountedfp, firstBlockPointer);
			
			// assignment to fp;
			fp[fpIndex] =  blockFilePtr; 
			fpInfo[fpIndex]= i;
			fclose(mountedfp);
			return i;
		}
	}
	else  // did not find in existing fat 
	{
		int j;
		for (j =0; j< 20; j++) // finding an empty spot in FAT
		{
			if(fat[j] == NULL)
			{
				//  create new FAT object 
				FileIndexObject* newFile;
				newFile = malloc(sizeof(FileIndexObject));
				newFile->filename = name;
				int s;
				for (s=0;s<10;s++){newFile->blockPtrs[s] =-1;}
				newFile->current_location=-1;

				// add to directory 
				fat[j] = newFile;
				PARTITION->numOfFiles ++;
				isFound =0;
				return j;
			}
		}
	

		if (isFound == -1) // didn't find empty slot in fat
		{
			return -1; // an error occured
		}
	}

	return -1; // should not reach here
}


int readBlock(int fileFATNum)
{
//  PURPOSE: using the file FAT index number, load buffer with data from blockID
	if(fileFATNum< 0 || fileFATNum > 19)
	{
		return -1;
	}
	FileIndexObject *current = fat[fileFATNum];

	return 1;
}
// char *returnBlock();
// int writeBlock(int file, char *data);




int main()
{
	initIO();
	partition("test.txt", 5, 40);
	initIO();


	FILE* correctFP;
	correctFP = mount("./PARTITION/test.txt");
	printf("%s\n",mountPartitionName);

	int j=0;
	
	int result = openfile("BTS");
	printf("openfile() result: %d\n",result );

	c= fgetc(fp[j]);
	printf("the char is: %c\n",c );

		

	// check if return the currect index 
	// check if has correct file pointer 

	return 0;
}