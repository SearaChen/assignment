/*
* Note: yet to handle nested commands, if want to, need to be able to EXTRACT the string value when doing print from memory
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "kernel.h"
#include "interpreter.h"
#include "shell.h"
#include "ram.h"


#define MAXCHAR 10000

int help()
{
	return 1;
}

int quit()
{
	printf("Bye!\n");
	//free(cpu);
	exit(EXIT_SUCCESS);
}

int set(char* varName, char* value) // set VAR STRING
{
	writeToMemory(varName, value);
	return 1;
}

int print(char* varName)
{
	printFromMemory(varName);
	return 1;
}

int run(char* filepath)
{
	char *line;
	char **args;
	char str[MAXCHAR];

	FILE *fp;

	if( access( filepath, F_OK ) != -1 ) {
    	fp = fopen(filepath, "r");
    	while (fgets(str, MAXCHAR, fp) != NULL )
    	{
    		args = parse(str);
    		execute(args);
    	}
    	fclose(fp);
	} else {
    	printf("Script not found!\n");
	}
	return 1;
}

int exec(int commandWordCount, char **args)
//int exec()
{
	int i =1; 
	/*FILE *fp ;
	FILE *fp2 ;
	FILE *fp3 ;

	fp = fopen("test1.txt", "r");
	launcher(fp);
	fp2 = fopen("test2.txt", "r");
	launcher(fp2);
	fp3 = fopen("test3.txt", "r");
	launcher(fp3);*/

	// queue all programs
	while (i < commandWordCount)
	{
		FILE *fp ;
		if( access( args[i], F_OK ) == -1 )
		{
			printf("Script %s not found!\n",args[i]);
			i++;
		}
		else
		{
	    	fp = fopen(args[i], "r");
	    	launcher(fp);

			i++;
		}
	}

	if (READYHEAD ==  NULL )
	{
		printf("why is ready null?\n");
		exit(EXIT_SUCCESS);
	}
	
	// performing robin-go-round
	while(READYHEAD != (ReadyQueueNode *) NULL && READYHEAD !=  NULL )
	{	
		int status;
		status = scheduler();

		if (status == -1)
		{
			printf("CPU is somehow not available error!");
		}
		//robin_go_count++;

	}

	return 1;
}

int interpret(char **args)
{
	// counting the number of words input in a line
	int commandWordCount= 0;
	int status;
	while(args[commandWordCount] != NULL)
	{
		commandWordCount++;
	}

	// it is an empty line
	if (commandWordCount == 0)
	{
		return 1;
	}

	if (strcmp(args[0],"help") == 0)
	{
		status = help();
		if (status == 1)
		{
			printf("help: Displays all the commands\nquit: Exits / terminates the shell with \"Bye!\"\nset: Assigns a value to shell memory\nprint: Prints the STRING assigned to VAR\nrun: Executes the file SCRIPT.TXT\nexec: execute up to 3 programs at the same time\n");
		}
		return status;
	}
	else if(strcmp(args[0],"quit") == 0)
	{
		status = quit();  // the program actually never goes here 
		return status;
	}
	else if(strcmp(args[0],"set") == 0 && commandWordCount == 3)
	{
		status = set(args[1], args[2]);
		return status;
	}
	else if (strcmp(args[0],"print") == 0 && commandWordCount == 2)
	{
		status = print(args[1]);
		return status;
	}
	else if (strcmp(args[0],"run") == 0 && commandWordCount == 2)
	{
		status= run(args[1]);
		return status;
	}
	else if(strcmp(args[0],"exec") == 0 && (commandWordCount > 1 && commandWordCount < 5))
	{
		status= exec(commandWordCount, args);
		//status= exec();
		return 1;
		
	}
	else
	{
		printf("Unknown command\n");
		return 1;
	}

}
