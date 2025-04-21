#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct 
{
    char** args;
    int argCount;
}parseInfo;

//functions that read the user input
char* readCommand();
parseInfo* parse(char* cmdLine);
void freeParseInfo(parseInfo* info);

//running the commands
void executeCommand(parseInfo* info);


#endif