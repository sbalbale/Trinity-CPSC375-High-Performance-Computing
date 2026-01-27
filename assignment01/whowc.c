/**
* File: whowc.c
* Purpose: Connects who and wc using a pipe
* Author: Sean Balbale
* Date: 1/28/2026
**/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("Pipe");
        exit(1);
    }
    switch (fork())
    {
    case -1:
        perror("Fork");
        exit(2);
    case 0: /* child */
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("wc", "wc", (char *)0);
        exit(3);
    default: /* parent */
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("who", "who", (char *)0);
        exit(4);
    }
    return 0;
}
