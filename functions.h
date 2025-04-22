#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct 
{
    char** args;
    int argCount;
}parseInfo;

//parsing the user input
char* readCommand();
parseInfo* parse(char* cmdLine);
void freeParseInfo(parseInfo* info);

//running the commands
void executeCommand(parseInfo* info);


#endif