#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#ifndef PATH_MAX              
#define PATH_MAX 4096         
#endif
#include "functions.h"

#define MAX_ARGS 64

void runExternal(char** argv){
    execvp(argv[0],argv);
    perror(argv[0]);
    exit(1);
}

static void runPwd() {
    char wd[PATH_MAX];
    puts(getcwd(wd, sizeof wd));
    exit(0);
}
//take care the getline
char* readCommand(){
    char* line=NULL;
    size_t len=0;
    if (getline(&line,&len,stdin)==-1)
    {
        puts("exit");
        exit(0);
    }
    return line;
    
}
//tokenization
parseInfo *parse(char *line) {
    parseInfo *pi = malloc(sizeof(*pi));
    pi->args = malloc(sizeof(char*) * (MAX_ARGS+1));
    pi->argCount = 0;

    char *tok = strtok(line, " \t\n");
    while (tok && pi->argCount < MAX_ARGS) {
        pi->args[pi->argCount++] = tok;
        tok = strtok(NULL, " \t\n");
    }
    pi->args[pi->argCount] = NULL;
    return pi;
}

void freeParseInfo(parseInfo *info) {
    free(info->args);
    free(info);
}
//take care in the builtin actions
static int builtin(parseInfo* info){
    if(!info->argCount){return 1;}
    char* cmd =info->args[0];

    if(!strcmp(cmd,"cd")){
        if (info->argCount<2) fprintf(stderr, "cd: missing path\n");
        else if (chdir(info->args[1])) perror("cd");
        return 1;
    }
    if (!strcmp(cmd, "pwd"))  { char wd[PATH_MAX]; puts(getcwd(wd,sizeof wd)); return 1; }
    if (!strcmp(cmd, "clear")){ system("clear");                       return 1; }
    if (!strcmp(cmd, "exit")) { exit(0);                                }
    if (!strcmp(cmd, "rmdir")){
        if (info->argCount<2) fprintf(stderr,"rmdir: missing dir\n");
        else if (rmdir(info->args[1])) perror("rmdir");
        return 1;
    }
    return 0; // not built‑in
}
//execute Command like pip redirect and execvp
void executeCommand(parseInfo *pi) {
    if (!pi->argCount) return;              // blank line

    if (pi->argCount && !strcmp(pi->args[0], "tree")) {
        if (access("tree", X_OK) != 0) {
            fprintf(stderr, "tree: command not found. Please compile with: gcc -o tree tree.c\n");
            return;
        }
        execvp("./tree", pi->args);
        perror("execvp failed----");
        return;
    }    

    // locate |  >  >>
    int pipeIndex=-1, redirIndex=-1, append=0;
    for (int i=0;i<pi->argCount;i++) {
        if (!strcmp(pi->args[i],"|"))  { pipeIndex=i; break; }
        if (!strcmp(pi->args[i],">") || !strcmp(pi->args[i],">>")) {
            redirIndex=i; append=(pi->args[i][1]=='>'); break;
        }
    }

    // single pipe 
    if (pipeIndex != -1) {
        pi->args[pipeIndex] = NULL;          
        char **right = &pi->args[pipeIndex+1];
        int fd[2]; pipe(fd);

        if (!fork()) {                       // first child
            dup2(fd[1],STDOUT_FILENO);
            close(fd[0]); close(fd[1]);
            if (!strcmp(pi->args[0],"pwd")) runPwd(); // built‑in pwd
            runExternal(pi->args);
        }
        if (!fork()) {                       // second child (external)
            dup2(fd[0],STDIN_FILENO);
            close(fd[0]); close(fd[1]);
            runExternal(right);
        }
        close(fd[0]); close(fd[1]); wait(NULL); wait(NULL);
        return;
    }

    // output redirection 
    if (redirIndex != -1) {
        pi->args[redirIndex] = NULL;
        char *file = pi->args[redirIndex+1];
        int flags = O_CREAT|O_WRONLY|(append?O_APPEND:O_TRUNC);
        int out = open(file, flags, 0644);
        if (out<0){ perror("open"); return; }
        if (!fork()) { dup2(out,STDOUT_FILENO); close(out); runExternal(pi->args); }
        close(out); wait(NULL); return;
    }

    // no pipe/redirect 
    if (builtin(pi)) return;           // built‑in handler
    if (!fork()) runExternal(pi->args);
    wait(NULL);
}
