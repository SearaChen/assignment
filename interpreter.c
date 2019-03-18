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

//int exec(int commandWordCount, char **args)
int exec()
{
	printf("are we executing this?\n");
	int i =1; 
	FILE *fp ;
	fp = fopen("test1.txt", "r");
	launcher(fp);

	// queue all programs
	/*while (i < commandWordCount)
	{
		FILE *fp ;
		if( access( args[i], F_OK ) == -1 )
		{
			printf("Script %s not found!\n",args[i]);
			i++;
		}
		else
		{
			// check for double loading 
			int doubleLoaded = -1; // default to not double loaded
			int j;
			for(j=1; j < i; j++)
			{
				if (strcmp(args[j],args[i]) == 0)
				{
					doubleLoaded =1;
					break;
				}
			}
			if (doubleLoaded ==1)
			{
				printf("Error: Script %s already loaded\n", args[j]);
				i++;
				continue;
			}
	    	fp = fopen(args[i], "r");
	    	launcher(fp);

			i++;
		}
	}*/

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

	// checking if all RAM are empty 
	// int in;
	// while (in < 10)
	// {
	// 	if (ram[in]!= NULL)
	// 	{
	// 		printf("the ram is not empty!\n");
	// 	}
	// 	in++;
	// }

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
			printf("help: Displays all the commands\nquit: Exits / terminates the shell with \"Bye!\"\nset: Assigns a value to shell memory\nprint: Prints the STRING assigned to VAR\nrun: Executes the file SCRIPT.TXT\n");
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
	else if(strcmp(args[0],"exec") == 0 && (commandWordCount > 0 && commandWordCount < 5))
	{
		//status= exec(commandWordCount, args);
		status= exec();
		return 1;
		
	}
	else
	{
		printf("Unknown command\n");
		return 1;
	}

}
