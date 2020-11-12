/* 
title: System Programming LAB3 Project
file name: shell.c
author: github_K-moovie, github_SWKANG0525
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int getargs(char *cmd, char **argv);

int main()
{
    char buf[256];
    char *argv[50];
    int narg;
    pid_t pid;
    while (1)
    {
        printf("shell> ");
        gets(buf);
        //  Q1. exit 입력 시 프로그램 종료.
        if(!strcmp(buf,"exit") || !strcmp(buf,"exit;")){
            break;
        }

        narg = getargs(buf, argv);
        pid = fork();
        if (pid == 0)
            execvp(argv[0], argv);
        else if (pid > 0)
            wait((int *)0);
        else
            perror("fork failed");
    }
}
int getargs(char *cmd, char **argv)
{
    int narg = 0;
    while (*cmd)
    {
        if (*cmd == ' ' || *cmd == '\t')
            *cmd++ = '\0';
        else
        {
            argv[narg++] = cmd++;
            while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t')
                cmd++;
        }
    }
    argv[narg] = NULL;
    return narg;
}
