#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "functions.h"

int main(int argc, char **argv) {
    int childPid; char *cmdLine; parseInfo *info;

    while (1) {
        printf("> "); fflush(stdout);
        cmdLine = readCommand();
        info    = parse(cmdLine);
        
        if(info->argCount && !strcmp(info->args[0], "exit")){executeCommand(info);}
        if(info->argCount && !strcmp(info->args[0], "cd"))
        {executeCommand(info); free(cmdLine); freeParseInfo(info); continue; }

        childPid = fork();
        if (childPid == 0) {
            executeCommand(info);      // child
            exit(0);
        } else {
            waitpid(childPid, NULL, 0); // parent waits
        }
        free(cmdLine); freeParseInfo(info);
    }
    return 0;
}