/*
 * File: mypipen.c
 * Purpose: Connects n commands using pipes (cmd1 | ... | cmdn)
 * Author: Sean Balbale
 * Date: 1/28/2026
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s cmd1 cmd2 ... cmdn\n", argv[0]);
        exit(1);
    }

    int i;
    int prev_pipe_read = STDIN_FILENO; // Initial input is stdin
    int fd[2];

    for (i = 1; i < argc; i++) {
        // Create pipe for next connection, unless it's the last command
        // We need a pipe between command i and i+1
        int is_last = (i == argc - 1);

        if (!is_last) {
            if (pipe(fd) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        switch (fork())
        {
        case -1:
            perror("Fork");
            exit(1);
        case 0: /* child */
            // Setup input: read from previous pipe (if not first command)
            // If prev_pipe_read is STDIN, we just leave STDIN alone.
            if (prev_pipe_read != STDIN_FILENO) {
                dup2(prev_pipe_read, STDIN_FILENO);
                close(prev_pipe_read);
            }

            // Setup output: write to current pipe (if not last command)
            if (!is_last) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]); // Close read end of new pipe in child
            }

            // Execute command
            execlp("sh", "sh", "-c", argv[i], (char *)NULL);
            perror("exec");
            exit(1);
        default: /* parent */
            // Close previous pipe read end (if not stdin)
            if (prev_pipe_read != STDIN_FILENO) {
                close(prev_pipe_read);
            }

            // If not last command, save current pipe read end for next iteration
            // and close write end
            if (!is_last) {
                close(fd[1]);
                prev_pipe_read = fd[0];
            }
            break;
        }
    }

    // Wait for all children
    while (wait(NULL) > 0);

    return 0;
}
