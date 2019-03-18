#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "ram.h"
#include "memorymanager.h"


//FILE *ram[RAMSIZE]; 

FILE* copyFile(FILE *fp1)
{
	//new file name 
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir("./BackingStore"); /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL) {
	    if (entry->d_type == DT_REG) { /* If the entry is a regular file */
	         file_count++;
	    }
	}

	// copy the files into new file 
	char* filepath[100];
	char snum[5];
	strcpy(filepath, "./BackingStore/");
	sprintf(snum,"%d",file_count);
	strcat(filepath,snum);
	strcat(filepath, ".txt");

	// new file creation
	FILE *backingP;
	backingP = fopen(filepath, "w+");

	char c;
 
    c = fgetc(fp1); 
    //xit(EXIT_SUCCESS);
    while (c != EOF) 
    { 
        fputc(c, backingP); 
        c = fgetc(fp1); 
    } 

	fclose(fp1);
	return backingP;
}


int countTotalPages(FILE *fp)
// count number of pages needed by a file program
{
	char ch;
	int lines = 0; 
	while(!feof(fp))
	{
		ch = fgetc(fp);
		if(ch == '\n')
		{
			lines++;
		}
	}
	fseek(fp, 0, SEEK_SET);

	int pageNum =0;
	pageNum = (lines + PAGESIZE - 1) / PAGESIZE;
	return pageNum;
}


FILE* findPage(int pageNumber, FILE* f)
{
	// return the file pointer at the beginning of the page that is indicated by the page Number 
	close(f);
	exit(EXIT_SUCCESS);

	FILE *fp2 = fdopen (dup (fileno (f)), "r");

	if(f==NULL){printf("findPage first file is null!\n");}
	if(fp2==NULL){printf("findPage second file is null!\n");}
	rewind(f);
	rewind(fp2);
	int pageCount=0;

	int lineCount = 0;
    char buffer[1000];
    printf("your?\n");
    	exit(EXIT_SUCCESS);

    while(pageCount != pageNumber && fgets(buffer, sizeof(buffer), fp2)!=  NULL)
    {
        lineCount++;
        printf("lineCount: %d\n",lineCount);
        printf("pageCount: %d\n",pageCount);

        printf("%s", buffer);
        printf("%ld\n",ftell(fp2));
        printf("\n");

       	if (lineCount == PAGESIZE)
       	{
       		pageCount++;
       		if (pageCount == pageNumber)
       		{
       			break;
       		}
       		else
       		{
	       		lineCount = 0;
	       		continue;
	       	}
       	}
    }

	return fp2;
}



int same_file_helper(int fd1, int fd2) {
    struct stat stat1, stat2;
    if(fstat(fd1, &stat1) < 0) return -1;
    if(fstat(fd2, &stat2) < 0) return -1;
    return ((stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino));
}


int isSameFilePointer(FILE*f1, FILE* f2)
{
	int fd1 = fileno(f1);
	int fd2 = fileno(f2);
    struct stat stat1, stat2;
    if(fstat(fd1, &stat1) < 0) return -1;
    if(fstat(fd2, &stat2) < 0) return -1;
    if ((stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino))
	{
		if (ftell(f1) == ftell(f2))
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}


int findFrame(FILE* page)
{
	int i;
	for(i=0;i<RAMSIZE;i++)
	{
		if (isSameFilePointer(page, ram[i]) == 1)
		{
			return i;
		}
	}

	return -1;
}


int findVictim(PCB*p)
{

	srand(time(NULL));   // Initialization, should only be called once.
	int r = rand();
	
	int count;
	for (count =0; count > RAMSIZE; count ++){ // loop through all possible slots
		r= r % RAMSIZE; 

		int i;
		int duplicate = -1;
		for (i = 0; i < RAMSIZE; i++) //  make sure it is not any where on the pageTable
		{
			if (isSameFilePointer(ram[i], p->pageTable[r]) == 1) // if it is already in use
			{
				duplicate=1;
			} 
		}

		if(duplicate == -1 ) // there is no duplicate, 
		{
			return r;
		}
		else
		{
			r++; 
		}
	}
}


int updateFrame(int frameNumber, int victimFrame, FILE *page)
{
	if(frameNumber == -1)
	{
		ram[victimFrame] = page;
	}
	else
	{
		ram[frameNumber] = page;
	}
	return 1; // dont really understand why are we returning 
}


int updatePageTable(PCB *p, int pageNumber, int frameNumber)
{
	p->pageTable[pageNumber] = frameNumber; 
}


int launcher(FILE *p) // called by exec command
{
	// count number of files 
	FILE* backingPtr;
	backingPtr=copyFile(p);

	if (backingPtr == NULL) // apparently not null 
	{
		printf("backingPtr is null\n");
	}


	fseek(backingPtr, 0, SEEK_SET);
	printf("rillldfap?\n");
	int num_pages=countTotalPages(backingPtr);
	printf("num of pages: %d\n",num_pages);
	addToRAM(p); // add first page
	if (num_pages>1)
	{
		// printf("rilllp?\n");
	 //    	exit(EXIT_SUCCESS);
		FILE* nextpage = findPage(1, p); // looking for the second page
		
		// printf("rilllp?\n");
    	exit(EXIT_SUCCESS);
		addToRAM(nextpage);
	}
	printf("we have done something!!\n");
	//printRAM();
	return 1;

}

void printRAM()
{
	int i;
	for (i =0; i<10; i++)
	{
		printf("%d slot is: %ld\n",i,ftell(ram[i]));

	}
}

// void myinit(FILE* fp) //To be deleted 
// {
// 	addToRAM(fp);
// 	PCB* newPCB;
// 	newPCB = makePCB(fp);
// 	addToReady(newPCB);
// }

// int main()
// {
// 	FILE* p;
// 	p = fopen("./script.txt", "r");
	
// 	FILE* p2;
// 	p2 = findPage(1, p);
// 	//p2 = fopen("./script2.txt", "r");


// 	if (isSameFilePointer( p, p2) == 1)
// 	{
// 		printf("yay!");
// 	}
// 	else{printf("no!!!!");}
// 	//launcher(p);


// }