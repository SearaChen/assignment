#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "ram.h"
#include "memorymanager.h"


// problems: WE NEVER CLOSED THE FILE POINTERS
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
	//strcpy(filepath, "");
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

	rewind(backingP);
	rewind(fp1);
	return backingP;
}


int countTotalPages(FILE *fp)
// count number of pages needed by a file program
{
	printf("running countTotalPages!\n");
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
	printf("LIENS: %d\n", lines);

	pageNum = (lines + PAGESIZE - 1) / PAGESIZE;
	printf("%d\n", pageNum);
	//exit(EXIT_SUCCESS);
	return pageNum;
}


FILE* findPage(int pageNumber, FILE* f)
{
	// return the file pointer at the beginning of the page that is indicated by the page Number 
	//close(f);

	FILE *fp2 = fdopen (dup (fileno (f)), "r");

	if(f==NULL){printf("findPage first file is null!\n");}
	if(fp2==NULL){printf("findPage second file is null!\n");}
	//rewind(f);
	rewind(fp2);
	int pageCount=0;

	int lineCount = 0;
    char buffer[1000];

    while(pageCount != pageNumber && fgets(buffer, sizeof(buffer), fp2)!=  NULL)
    {
        lineCount++;

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


int findFrame(FILE* page) // NOT ACTUALLY USED?
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
// int findFrame(pageNumber)
// {
// 	int i;
// 	for(i=0;i<RAMSIZE;i++)
// 	{
// 		if (isSameFilePointer(page, ram[i]) == 1)
// 		{
// 			return i;
// 		}
// 	}

// 	return -1;
// }


int findVictim(PCB*p)
{
	// NOTE:  will not work if a process has 10 pages already in RAM 
	srand(time(NULL));   // Initialization, should only be called once.
	int r = rand();
	
	int count;
	for (count =0; count > RAMSIZE; count ++){ // loop through all possible slots
		r= r % RAMSIZE; 

		int i;
		int duplicate = -1;
		for (i = 0; i < RAMSIZE; i++) //  make sure it is not any where on the pageTable
		{
			if (ram[i]!= NULL && isSameFilePointer(ram[i], p->pageTable[r]) == 1) // if it is already in use
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


void printRAM()
{
	int i;
	for (i =0; i<10; i++)
	{
		printf("%d slot is: %p\n",i,(ram[i]));

	}
}

void printPCBTable(PCB* pcb)
{
	int i;
	for (i=0;i<10;i++)
	{
		printf("At page index: %d -- %d\n",i,pcb->pageTable[i]);
	}
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
	int num_pages=countTotalPages(backingPtr);
	printf("num of pages: %d\n",num_pages);
	int firstPageFrameNumber=addToRAM(backingPtr); // add first page

	int secondPageFrameNumber=-1;
	if (num_pages>1)
	{
		FILE* nextpage = findPage(1, backingPtr); // looking for the second page
		secondPageFrameNumber= addToRAM(nextpage);
	}

	printRAM();

	// creating 1 pcb for 1 process (not 1 page)
	PCB* newPCB;
	newPCB = makePCB(backingPtr);
	if (secondPageFrameNumber <0 && num_pages > 1)
	{
		printf("Memory manager: Second page frame number smaller than zero!\n");
	}

	updatePageTable(newPCB, 0, firstPageFrameNumber);
	if (secondPageFrameNumber >=0)
	{
		updatePageTable(newPCB, 1, secondPageFrameNumber);
	}

	printPCBTable(newPCB);
	newPCB->pages_max=num_pages;
	printf("launcherdone! \n");

	addToReady(newPCB);
	printf("PCB added to queue!");
	return 1;

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