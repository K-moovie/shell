/* 
title: System Programming LAB3 Project
file name: shell.c
author: github_K-moovie, github_SWKANG0525
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <sys/resource.h>
#include <fcntl.h>
int getargs(char *cmd, char **argv);
void launch(int narg, char **argv);

int main()
{
    char buf[256];
    char *argv[50];
    int narg;
    pid_t pid;
    int i = 0;
    while (1)
    {
        for(i=0; i <50; i++){
             argv[i] = '\0';
        }
        printf("shell> ");
	    gets(buf);
        //  Q1. exit 입력 시 프로그램 종료.
        if(!strcmp(buf,"exit") || !strcmp(buf,"exit;")){
            break;
        }

        narg = getargs(buf, argv);
        launch(narg, argv);
        
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

void launch(int narg, char **argv)
{
    pid_t pid = 0;
    int i = 0;
    int is_background = 0;
    if(narg != 0 && !strcmp(argv[narg -1], "&") ){
        argv[narg-1] = NULL;
        is_background = 1;
    }

    pid = fork();
    if (pid == 0){
        if(is_background){
            printf("\nCREATE BACKGROUND PROCESS PID: %ld\n", (long)getpid());
        }

        if(execvp(argv[0], argv) < 0) {
            perror("BACKGROUND:");
        }
    }
    
    else {
        if(is_background == 0){
            wait(pid);
        }
    }
}
