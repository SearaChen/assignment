#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"

#define RL_BUFSIZE 1024

char *read_line()
{	
	int bufsize = RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char)*bufsize);
	int c;

	if (!buffer)
	{
		fprintf(stderr, "allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		c = getchar();

		if (c == EOF || c == '\n')
		{
			buffer[position] = '\0';
			return buffer;
		}
		else
		{
			buffer[position] = c;
			position++;
		}

		// recreate buffer if exceed 
		if (position >= bufsize)
		{
			bufsize += RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) 
			{
				fprintf(stderr, "allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


#define TOK_BUFSIZE 64
#define TOK_DELIM " \n"
char **parse(char *line)
{
	int bufsize = TOK_BUFSIZE;
	int position =0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) 
	{
	    fprintf(stderr, "lsh: allocation error\n");
	    exit(EXIT_FAILURE);
  	}

  	token = strtok(line,TOK_DELIM);
  	while(token != NULL) // extract all the tokens
  	{
  		tokens[position] = token;
  		position++;

  		if (position >= bufsize)
  		{
  			bufsize += TOK_BUFSIZE;
      		tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) 
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
  		}
  		token = strtok(NULL, TOK_DELIM);
  	}

  	tokens[position] = NULL;
  	return tokens;
}

int execute(char **args)
{
	int position = 0;
	interpret(args);
	return 1;
}


void lsh_loop()
{
	char *line;
	char **args;
	int status; 

	status = 1;
	int inputLength=2;

	while (status) // should equal to 1 for it to continue
	{
		if (inputLength > 1 ){
			printf("$");
		}
		else
		{
			freopen("/dev/tty", "r", stdin);
		}

		line = read_line();
		inputLength = strlen(line);
		args = parse(line);
		status = execute(args);
		free(line);
		free(args);

	} 
}


void UIInstantiate()
{
   printf("Kernel 1.0 loaded!\nWelcome to the Seara Chen shell!\nShell version 3.0 Updated March 2019\n");
   lsh_loop();
}