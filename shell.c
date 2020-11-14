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
#define BUFSIZE 256

pid_t pid;
int getargs(char *cmd, char **argv);
void launch(int narg, char **argv);
int getargs(char *cmd, char **argv);
void SIGINT_Handler(int signo);
void SIGQUIT_Handler(int signo);
void pipe_launch(char **argv,int narg);
char *substring(int start, int end, char * str);
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
        printf("shell> ");
	    gets(buf);
        //  Q1. exit 입력 시 프로그램 종료.
        if(!strcmp(buf,"exit") || !strcmp(buf,"exit;")){
            break;
        }

        narg = getargs(buf, argv);
        launch(narg, argv);
        //redirection(narg, argv);

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

void pipe_launch(char **argv,int narg) {
    int command_pos = 0;
    int count_pipe = 0;
    int i  = 0 ;
    int pd_idx = 0;
    int k = 0;
    pid_t pid;
    int status;

    while(argv[command_pos] != NULL) {
        if(argv[command_pos][0] =='|')
            count_pipe ++;
        command_pos++;
    }

    int fd[count_pipe*2];
    for(i = 0; i < (count_pipe); i++){
        if(pipe(fd + i*2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    command_pos = 0;
    for( i = 0 ; i<=count_pipe; i++) {
        int j =0;
        char **command = (char **)malloc(narg * sizeof(char *));

        if(!strcmp(argv[command_pos], "|"))
            command_pos++;

        if(i <count_pipe) {
        while(strcmp(argv[command_pos], "|")  ) {
            command[j] = (char *)malloc(100 * sizeof(char));
            strcpy(command[j],argv[command_pos]);
            command_pos++;
            j++;
            }
        }
        
         else {
            while(argv[command_pos] != NULL) {
                command[j] = (char *)malloc(100 * sizeof(char));
                strcpy(command[j],argv[command_pos]);
                command_pos++;
                j++;

            }
        }

        command[j] = NULL;

        pid = fork();
        if(pid == 0) {
            if(i<count_pipe) {
            if(dup2(fd[pd_idx + 1], 1) < 0){
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            }

            if(pd_idx != 0 ){
                if(dup2(fd[pd_idx-2], 0) < 0){
                    perror(" dup2");
                    exit(EXIT_FAILURE);

                }
            }
            for(k = 0; k < 2*count_pipe; k++){
                close(fd[k]);
            }

            if( execvp(command[0], command) < 0 ){
                    perror(command);
                    exit(EXIT_FAILURE);
            }
        }
        pd_idx+=2;

        k=0;
        while(command[k] != NULL) {
            free(command[k]);
            k++;
        }
        free(command);
    }

    for(k = 0; k < 2 * count_pipe; k++){
        close(fd[k]);
    }

    for(k = 0; k < count_pipe + 1; k++)
        wait(&status);
    
}

void launch(int narg, char **argv)
{
    int is_pipe= 0;
    pid_t pid = 0;
    int i = 0;
    int is_background = 0;
    if(narg != 0 && !strcmp(argv[narg -1], "&") ){
        argv[narg-1] = NULL;
        is_background = 1;
    }

    for(i = 0; i<narg; i++)
        if(!strcmp(argv[i],"|"))
            is_pipe = 1;    

    if(is_pipe == 1)
        pipe_launch(argv,narg);

    
    else {
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
}

char *substring(int start, int end, char * str) {
    char *new = (char *)malloc(sizeof(char)*(end-start+2));
    strncpy(new,str+start,end-start+1);
    new[end-start+1] = 0;
    return new;
}
