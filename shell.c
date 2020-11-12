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
pid_t pid;
int getargs(char *cmd, char **argv);
void launch(int narg, char **argv);
void redirection(int narg, char **argv);
int getargs(char *cmd, char **argv);
void SIGINT_Handler(int signo);
void SIGQUIT_Handler(int signo);
void SIGQUIT_Handler(int signo) {
    printf("\n");
    exit(1);
}

void SIGINT_Handler(int signo)
{   
    if(pid == 0) {
        printf("\n");
        exit(1);
    }
}
int main()
{
    signal(SIGINT, SIGINT_Handler);
    signal(SIGTSTP, SIGQUIT_Handler);
    char buf[256];
    
    int narg;
    int i = 0;
    while (1)
    {
        char *argv[50] = {'\0'} ;
        //for(i=0; i <50; i++){
        //     argv[i] = '\0';
        //}
        printf("shell> ");
	    gets(buf);
        //  Q1. exit 입력 시 프로그램 종료.
        if(!strcmp(buf,"exit") || !strcmp(buf,"exit;")){
            break;
        }

        narg = getargs(buf, argv);
        //launch(narg, argv);
        redirection(narg, argv);

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
            perror("[ERROR] CREATE BACKGROUND: ");
        }
    }
    
    else {
        if(is_background == 0){
            wait(pid);
        }
    }
}

void redirection(int narg, char **argv) {
    pid_t pid;
    int i = 0;
    int fd;
    int split_index = 0, is_write = 0;

    int write_flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    char *cmd[10] = {'\0'}; // redirection을 수행할 명령어 저장.


    for(i = 0; i < narg; i++){
        if(!strcmp(argv[i], ">")){
            split_index = i;
            is_write = 1;
        }
        else if(!strcmp(argv[i], "<")){
            split_index = i;
            is_write = 0;
        }
    }

    for(i = 0; i < split_index; i++){
        cmd[i] = argv[i];
    }

    pid = fork();
    if(pid == 0) {
        // > 연산자 
        if (is_write){
            if ((fd = open(argv[split_index + 1], write_flags, mode)) == -1) {
                perror("[ERROR] OPEN: ");
                exit(1);
            }
            if (dup2(fd, 1) == -1) {
                perror("[ERROR] DUP2: ");
                exit(1);
            }
        }
        // < 연산자
        else{
            if ((fd = open(argv[split_index + 1], O_RDONLY)) == -1) {
                perror("[ERROR] OPEN: ");
                exit(1);
            }
            if (dup2(fd, 0) == -1) {
                perror("[ERROR] DUP2: ");
                exit(1);
            }
        }
    
        if (close(fd) == -1) {
            perror("[ERROR] CLOSE: ");
            exit(1);
        }
        execvp(cmd[0], cmd);
    }

    else if (pid > 0) {
        wait(pid);
    }
}