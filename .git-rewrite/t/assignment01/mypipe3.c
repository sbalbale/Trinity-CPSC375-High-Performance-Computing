/*
 * File: mypipe3.c
 * Purpose: Connects 3 commands using pipes (cmd1 | cmd2 | cmd3)
 * Author: Sean Balbale
 * Date: 1/28/2026
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // Expect exactly 3 commands
    if (argc != 4) {
        fprintf(stderr, "Usage: %s cmd1 cmd2 cmd3\n", argv[0]);
        exit(1);
    }

    int fd1[2]; // Pipe 1
    int fd2[2]; // Pipe 2

    // Create pipe 1
    if (pipe(fd1) == -1) {
        perror("pipe1");
        exit(1);
    }

    // Fork first child (cmd1)
    switch (fork())
    {
    case -1:
        perror("Fork1");
        exit(1);
    case 0: /* Child 1 */
        // Output to pipe1 write end
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[0]);
        close(fd1[1]);
        
        // Execute cmd1
        execlp("sh", "sh", "-c", argv[1], (char *)NULL);
        perror("exec1");
        exit(1);
    default: /* parent */
        break;
    }

    // Create pipe 2
    if (pipe(fd2) == -1) {
        perror("pipe2");
        exit(1);
    }

    // Fork second child (cmd2)
    switch (fork())
    {
    case -1:
        perror("Fork2");
        exit(1);
    case 0: /* Child 2 */
        // Input from pipe1 read end
        dup2(fd1[0], STDIN_FILENO);
        // Output to pipe2 write end
        dup2(fd2[1], STDOUT_FILENO);

        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        // Execute cmd2
        execlp("sh", "sh", "-c", argv[2], (char *)NULL);
        perror("exec2");
        exit(1);
    default: /* parent */
        break;
    }

    // Close pipe 1 in parent as it's no longer needed by parent or future children
    close(fd1[0]);
    close(fd1[1]);

    // Fork third child (cmd3)
    switch (fork())
    {
    case -1:
        perror("Fork3");
        exit(1);
    case 0: /* Child 3 */
        // Input from pipe2 read end
        dup2(fd2[0], STDIN_FILENO);
        // Output to stdout (default)

        close(fd2[0]);
        close(fd2[1]);

        // Execute cmd3
        execlp("sh", "sh", "-c", argv[3], (char *)NULL);
        perror("exec3");
        exit(1);
    default: /* parent */
        break;
    }

    // Parent closes pipe 2
    close(fd2[0]);
    close(fd2[1]);

    // Wait for all children
    while (wait(NULL) > 0);

    return 0;
}
