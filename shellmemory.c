#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#include "shellmemory.h"

typedef struct node {
	int id; // not necessarity 
	char* name;
	char* value;
	struct node * next;
} Node;

Node* Head; 


void printMemory(Node* current)
{
	printf("----------starting to print ...-----------\n");
	while(current != (Node*)NULL)
	{
		printf("%d\n", current->id);
		printf("%s\n", current->name);
		printf("%s\n", current->value);
		current = current->next;
	}
	printf("-----------------------\n");
}

void writeToMemory(char* inputVariableName, char* inputValue)
{
	Node* current = Head;
	Node* previous = Head;
	int current_id=0;
	while(current != (Node*)NULL) // comparison making it segfault
	{
		if (strcmp(current->name,inputVariableName) == 0 )
		{
			free(current->value);
			current->value =(char *) malloc(sizeof(char)*strlen(inputVariableName)+5);
			strcpy(current->value ,inputValue);
			return;
		}
		previous=current;
		current = current->next;
		current_id++;
	}
	// it is a new variable
	Node* newNode= malloc (sizeof(Node));
	newNode->name = (char *) malloc(sizeof(char)*strlen(inputVariableName)+5);
    strcpy(newNode->name ,inputVariableName);
	
	newNode->value =(char *) malloc(sizeof(char)*strlen(inputVariableName)+5);
	strcpy(newNode->value ,inputValue);

	newNode->id = current_id;
	newNode->next = NULL;

	current = newNode; // adding to the list

	// check if this is the first one added to the list
	if (current_id == 0)
	{
		Head = current;
	}
	else
	{
		previous->next = current;
	}
}


void printFromMemory(char* inputVariableName)
{

	Node* current = Head;
	while(current != (Node*) NULL)
	{
		if (strcmp(current->name,inputVariableName) == 0)
		{
			printf("%s\n",current->value);
			return;
		}
		current = current->next;

	}

	// variable does not exit
	char*result;
	result="Variable does not exist";
	printf("%s\n",result);
	return; 
}