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
int block_buffer_index=0;
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

void wipe_block_buffer()
{
	free(block_buffer);
	block_buffer =  malloc(sizeof(char)*PARTITION->total_blocks*PARTITION->block_size);
}


FILE* giveBlockNumberContentPtr(FILE* f, int i)
{	
// 	PURPOSE: give the pointer that point to the ith block inside partition file f
// 	   RETURN: a NEW file ptr to the ith block
	// FILE *fpDup = fdopen (dup (fileno (f)), "r+");
	// int round = PARTITION->numOfFiles;
	// fseek(fpDup, (sizeof(Partition)+(round)*sizeof(FileIndexObject)+ (i*PARTITION->block_size)), SEEK_SET);
	// return fpDup;

	FILE *fpDup = fdopen (dup (fileno (f)), "r+");
	int j ;
	int offsetFilesCount =0;
	for(j =0;j<10; j++)
	{
		if(fat[j]!=NULL && fat[j]->file_length!=0)
		{
			offsetFilesCount++;
		}
	}
	fseek(fpDup, (sizeof(Partition)+(offsetFilesCount)*sizeof(FileIndexObject)+ (i*PARTITION->block_size)), SEEK_SET);
	return fpDup;

}


void printUserDataSection()
{
	FILE* f = fopen(mountPartitionName,"rb+");
	FILE* writePtr = giveBlockNumberContentPtr(f, 0);
	printf("User Data Section Content: \n");
	int c;
	int count=0;
	while((c=fgetc(writePtr)) != EOF)
	{
		printf("%c",c);
		count++;
		if(count % PARTITION->block_size == 0 )
		{
			printf(" ");
		}
	}
	printf("\n");
	fclose(writePtr);
	fclose(f);
}

void printGlobalOpenFilePointer()
{
	int i;
	printf("Printing global file pointers...\n");
	for(i=0;i<5;i++)
	{	
		if (fp[i] == NULL)
		{
			printf("null,");
			continue;
		}
		printf("%ld,", ftell(fp[i]));
	}
	printf("\n");
}

int isFileAlreadyOpen(char* filename)
{// Purpose: check if a file with "filename" is already open through looping through fp's
 // Return : its fat index if it is already open
 //			 -1 if it is not already opeN
	printf("check if the file already opened... \n");
	int i;
	for (i =0; i < 5; i++)
	{
		if(fp[i]!=NULL)
		{
			if (strcmp(fat[fpInfo[i]]->filename, filename) == 0)
			{
				return fpInfo[i];
			}
		}
	}
	return -1;
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

	// create the file with file name
	char partitionPath[200];
	strcpy(partitionPath, "./PARTITION/");
	strcat( partitionPath, name);
	FILE *partitionP; 

	//create partition
	PARTITION->total_blocks = totalblocks;
	PARTITION->block_size = blocksize;
	PARTITION->numOfFiles = 0;
	partitionP = fopen( partitionPath, "rb+");	

	// ===================================== TEST AREA =============================================
	int firstBlockPointer = 5;
	FileIndexObject * newFile = malloc(sizeof(FileIndexObject));
	newFile->filename = "BTS";
	newFile->file_length = 20;
	int s;
	for (s=0;s<10;s++){newFile->blockPtrs[s] =-1;}
	newFile->blockPtrs[0]= firstBlockPointer;
	newFile->current_location = 1;
	newFile->file_length=1;

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

		if (i == firstBlockPointer*blocksize) {
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
	return 0;
}

	


int mount(char *name)
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
		return -1; // error code 
	}


	f= fopen(name,"rb+");

	decodeGlobalVariableFromFile(f);
	
 	block_buffer = malloc(sizeof(char)*PARTITION->total_blocks*PARTITION->block_size);

 	strcpy(mountPartitionName, name);

	return 0;
}


int findAvailableSpotInfp()
{// PURPOSE: find an available spot (== NULL) in fpp
 // RETURN : -1 if not available spot found
			//  if found: the index 
	int fpIndex = 0;
	for(fpIndex=0; fpIndex<5;fpIndex++)
	{
		if (fp[fpIndex] == NULL)
		{
			break;
		}
	}

	if (fpIndex == 5)
	{
		return -1;
	}
	else
	{
		return fpIndex;
	}
}


int openfile(char *name)
{	// INPUT: the name of the file looking for
	//RETURN: file's FAT index , -1 when FAT/fp is full, -2 if a new file is created
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
		int fpIndex=findAvailableSpotInfp();

		if (fpIndex == -1){ // did not find available spot for fpIndex 
			return -1; //error
		}
		else
		{
			FILE * mountedfp;
			printf("first block pointer: %d\n", firstBlockPointer );
			mountedfp= fopen(mountPartitionName,"rb+");
			FILE* blockFilePtr = giveBlockNumberContentPtr(mountedfp, firstBlockPointer);
			
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
				newFile->current_location=0;
				newFile->file_length=0;

				// add to directory 
				fat[j] = newFile;
				PARTITION->numOfFiles ++;
				isFound =0;
				//return j;
				return -2;
			}
		}
	

		if (isFound == -1) // didn't find empty slot in fat
		{
			return -1; // an error occured
		}
	}

	return -1; // should not reach here
}


int readBlock(int fileFATIndex)
{
	//  PURPOSE: using the file FAT index number, load buffer with 1 block of data from blockID
	//	RETURN : 0 if success; 
	//		     -1 end of file error
	// 			 -3  unexpected error -- bug in the code
	// 	NOTE : If success -- global buffer_index_pointer +1
	//						fat entry current_position +1
	if(fileFATIndex< 0 || fileFATIndex > 19)
	{
		return -1;
	}

	FileIndexObject *currentFile = fat[fileFATIndex];

	//find filepointer in fpinfo
	int isFound =-1;
	int i;
	FILE* currentfp;
	for (i=0; i< 5; i++)
	{
		if(fpInfo[i] == fileFATIndex)
		{
			isFound=0;
			currentfp = fp[i];
			break;
		}
	}

	if(currentfp == NULL || isFound == -1)
	{
		printf("DISK_driver.c : Error! Corresponding fp not found to fileFATIndex!\n");
		return -3;
		//exit(EXIT_SUCCESS);
	}

	if (currentFile->current_location >= 10) // read through all 20 blockPtrs
	{
		printf("reached end of file for file, cannot read more: %s\n", currentFile->filename );
		return -1; // return error code
	}
	else 
	{
		int currentBlockNum= currentFile->blockPtrs[currentFile->current_location];
		if (currentBlockNum == -1) // has reached an invalid blockFilePtr
		{
			printf("reached end of file/some error blockFilePtR for file: %s\n", currentFile->filename );
			return -1; // return error code

		}

		// TODO: CHECK WHAT IS THE CORRECT WAY OF DOING THIS!!!
		wipe_block_buffer();
		int c;
		printf("Current block num: %d\n", currentBlockNum);
		FILE* blockPtr = giveBlockNumberContentPtr(currentfp, currentBlockNum);

		for (i=0;i<PARTITION->block_size; i++)
		{	
			c= fgetc(blockPtr);
			printf("block's %d char is: %c\n", i, c);
			//exit(EXIT_SUCCESS);

			block_buffer[i]=c;
		}
		printf("current block num: %d\n", currentBlockNum );
		printf("Content writing result is: %s\n", block_buffer);

		//block_buffer_index++;
		currentFile->current_location ++;
		return 0;

	}
	return -3; // should never reach here
}


char *returnBlock()
{// PURPOSE : return block data as string from block_buffer
	return block_buffer; // return the most recently written character in block buffer
}


int writeBlock(int fileFATIndex, char *data)
{	
	printf("writing data: %s\n", data);
	// TODO: FIGURE OUT  HOW ARE WE LOADING IT INTO THE BUFFER? 
	wipe_block_buffer();
	strcpy(block_buffer, data);

	if(fileFATIndex< 0 || fileFATIndex > 19)
	{
		return -1;
	}

	FileIndexObject *currentFile = fat[fileFATIndex];

	//find filepointer in fpinfo
	int isFound =-1;
	int i;
	FILE* currentfp;
	for (i=0; i< 5; i++)
	{
		if(fpInfo[i] == fileFATIndex)
		{
			isFound=0;
			currentfp = fp[i];
			break;
		}
	}

	if(isFound == -1)
	{
		printf("writeBlock(): Error! did not found FAT entry!\n");
		return -1;
	}
	else if (currentfp == NULL) // the corresponding filepointer is not open yet
	{
		// TODO: decide what is the correct thing to do
		printf("writeBlock(): Error! fpointer is null\n");
		return -1;
	}
	else
	{
		if (currentFile->current_location > 9) // read through all 10 blockPtrs , no empty pointer spot
		{
			printf("reached end of file for file, cannot write more : %s\n", currentFile->filename );
			return -1; // return error code
		}


		// actually writing characters 
		int c;
		int toWriteCh;
		FILE* writePtr= giveBlockNumberContentPtr(currentfp, 0); // beginning of the user data section
		int isWritten=-1;
		int j=0;
		int roundCounter =0;
		i=0;

		printf("=========== initial fat entry info ============ \n");
		printUserDataSection(); //TODO: for some reason that the second entry is not written? 
		printf("file's current_location:%d\n",currentFile->current_location );
		printf("file's pointers:\n");
		int s;
		for(s=0;s<10;s++)
		{
			printf("%d,",currentFile->blockPtrs[s]);
		}

		while(j < (PARTITION->total_blocks) && i < strlen(data) && currentFile->current_location < 10 ) // finding the next avaible empty block
		{
			
			c= fgetc(writePtr);
			printf("the char is: %c\n",c );
			if(c == '0') // it is an unused block
			{
				fclose(writePtr);
				writePtr = giveBlockNumberContentPtr(currentfp, 0);// rewind to beginning of the user data section
				fseek(writePtr,j*PARTITION->block_size, SEEK_CUR); // prepare to write at current position

				while( i < strlen(data) && roundCounter < PARTITION->block_size)
				{
					toWriteCh=data[i];
					printf("writing char: %c\n", toWriteCh);
					int result= fputc(toWriteCh, writePtr);
					// if (result!=toWriteCh)
					// {
					// 	printf("error!\n");
					// 	exit(EXIT_SUCCESS);
					// }
					i++;
					roundCounter++;
				}

				currentFile->blockPtrs[currentFile->current_location] = j;
				currentFile->current_location ++;
				// TODO: change file_length ?
				isWritten=0;
				printf("===========written one block at block# %d============ \n", j);
				printf("file's current_location:%d\n",currentFile->current_location );
				printf("file's pointers:\n");
				printUserDataSection();

				int s;
				for(s=0;s<10;s++)
				{
					printf("%d,",currentFile->blockPtrs[s]);
				}
			}

			if (isWritten == 0) // prepare for next round
			{
				isWritten=-1;
				roundCounter=0;
			}
			j++;

		}

		if (i < strlen(data) && isWritten==-1)
		{
			printf("Error: due to lack of space cannot write the whole thing!\n");
			return -1;
		}
		return 0;
	}
	return -1; // never should reach here
}


int main()
{
	initIO();
	partition("test.txt", 2, 10);
	initIO();

	// WRITING FILE TEST CASES
	mount("./PARTITION/test.txt");
	int openResult = openfile("EXO");
	openResult = openfile("EXO");
		printGlobalOpenFilePointer();

	openResult = openfile("BTS");
		printGlobalOpenFilePointer();



	printf("%d", isFileAlreadyOpen("BTS"));
	printGlobalOpenFilePointer();
	/*int openResult = openfile("BTS");
	printGlobalOpenFilePointer();
	printUserDataSection();

	//int writeResult= writeBlock(openResult,"BTSISTHEBEST!AND I ALSO LOVE EXO"); //"BTSISTHEBEST!"
	openResult = openfile("EXO");
	printGlobalOpenFilePointer();
	printUserDataSection();
	printf("openfile() result: %d\n",openResult );
	openResult = openfile("EXO");
	printGlobalOpenFilePointer();

	printf("openfile() result: %d\n",openResult );
	
	int readBlockResult=readBlock(openResult);
	printf("readBlock() result: %d\n",readBlockResult );
	printf("buffer result: %s\n", block_buffer);
	
	writeBlock(openResult,"exo comebacklalahaha");
	printUserDataSection();*/

	
	//READING FILE TEST CASES
	// test case 1: 
	/*int openResult = openfile("BTS");
	printf("openfile() result: %d\n",openResult );

	int readBlockResult=readBlock(openResult);
	printf("readBlock() result: %d\n",readBlockResult );

	printf("block Content: %s\n",block_buffer);

	// test case 2:
	printf("===================\n");
	openResult = openfile("RISE");
	printf("openfile() result: %d\n",openResult );

	readBlockResult=readBlock(openResult);
	printf("readBlock() result: %d\n",readBlockResult );*/


	return 0;
}