/*
 * File: client.c
 * Purpose: Implements a simple client for server-client chat using named pipes (FIFOs)
 * Author: Sean Balbale
 * Date: 2/6/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define FIFO1 "/tmp/seanb_FIFO1"
#define FIFO2 "/tmp/seanb_FIFO2"

int main()
{
    int fd1, fd2;
    char buf[BUFSIZ];
    pid_t pid;

    printf("Client connecting...\n");

    // Client opens the existing pipes
    // Must match the Server's order to avoid deadlock
    // Server: open(FIFO1, WR), open(FIFO2, RD)

    // 1. Open FIFO1 for reading
    // This unblocks the Server's open(FIFO1, WR)
    fd1 = open(FIFO1, O_RDONLY);
    if (fd1 == -1)
    {
        if (errno == ENOENT)
        {
            fprintf(stderr, "Error: FIFO1 not found. Start the server first.\n");
        }
        else
        {
            perror("open FIFO1");
        }
        exit(EXIT_FAILURE);
    }

    // 2. Open FIFO2 for writing
    // This unblocks the Server's open(FIFO2, RD)
    fd2 = open(FIFO2, O_WRONLY);
    if (fd2 == -1)
    {
        perror("open FIFO2");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Start typing. (Send '.' to exit)\n");

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        // Parent process: Monitors stdin and writes to the outgoing FIFO (FIFO2)
        while (1)
        {
            // Read from stdin
            if (fgets(buf, BUFSIZ, stdin) == NULL)
            {
                break;
            }

            // Write to FIFO2
            write(fd2, buf, strlen(buf) + 1);

            // Check for exit condition
            if (strcmp(buf, ".\n") == 0)
            {
                break;
            }
        }

        // Cleanup
        close(fd1);
        close(fd2);

        kill(pid, SIGTERM);
        wait(NULL);
        printf("Client exited.\n");
        exit(0);
    }
    else
    {
        // Child process: Blocks on the incoming FIFO (FIFO1) and renders received data
        while (1)
        {
            ssize_t n = read(fd1, buf, BUFSIZ); // Read from FIFO1
            if (n <= 0)
            {
                break; // EOF or error
            }

            // Check for exit condition from Server
            if (strcmp(buf, ".\n") == 0)
            { // Server signals exit
                printf("Server disconnected.\n");
                kill(getppid(), SIGTERM); // Notify parent to exit
                break;
            }

            // Print received message
            printf("Server: %s", buf);
            fflush(stdout);
        }

        close(fd1);
        close(fd2);
        exit(0);
    }

    return 0;
}
