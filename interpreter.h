#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
 

int help();
int quit();
int set(char* varName, char* value);
int print(char* varName);
int run(char* filepath);
//int exec(int commandWordCount, char **args);
int exec();
int interpret(char **args);
 
#endif