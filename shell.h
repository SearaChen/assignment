#ifndef _SHELL_H_
#define _SHELL_H_
 

char **parse(char *line);
int execute(char **args);
void UIInstantiate();
 
#endif