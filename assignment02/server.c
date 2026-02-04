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

#define FIFO1 "/tmp/FIFO1"
#define FIFO2 "/tmp/FIFO2"

void cleanup_and_exit(int signo) {
    printf("\nConversation ended.\n");
    unlink(FIFO1);
    unlink(FIFO2);
    exit(0);
}

int main() {
    int fd1, fd2;
    char buf[BUFSIZ];
    pid_t pid;

    // Handle termination signal to clean up pipes
    signal(SIGTERM, cleanup_and_exit);

    // Create FIFOs
    // The server must create the pipes
    if (mkfifo(FIFO1, 0666) == -1) {    // Create first FIFO
        if (errno != EEXIST) {          // Ignore error if FIFO already exists
            perror("mkfifo FIFO1");
            exit(EXIT_FAILURE);
        }
    }
    if (mkfifo(FIFO2, 0666) == -1) {    // Create second FIFO
        if (errno != EEXIST) {          // Ignore error if FIFO already exists
            perror("mkfifo FIFO2");
            exit(EXIT_FAILURE);
        }
    }

    printf("Server listening... (Waiting for client to connect)\n");

    // Open FIFOs - Order is critical for deadlock avoidance
    // Server writes to FIFO1 and reads from FIFO2
    
    // 1. Open FIFO1 for writing
    // This call blocks until the Client opens FIFO1 for reading
    fd1 = open(FIFO1, O_WRONLY);
    if (fd1 == -1) { 
        perror("open FIFO1"); 
        exit(EXIT_FAILURE); 
    }
    
    // 2. Open FIFO2 for reading
    // Client should open FIFO2 for writing around the same time
    fd2 = open(FIFO2, O_RDONLY);
    if (fd2 == -1) { 
        perror("open FIFO2"); 
        exit(EXIT_FAILURE); 
    }

    printf("Client connected. Start typing. (Send '.' to exit)\n");

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process: Monitors stdin and writes to the outgoing FIFO (FIFO1)
        while (1) {
            // Read from stdin
            if (fgets(buf, BUFSIZ, stdin) == NULL) {
                break;
            }

            // Write to FIFO1
            write(fd1, buf, strlen(buf) + 1);

            // Check for exit condition
            if (strcmp(buf, ".\n") == 0) {
                break;
            }
        }

        // Cleanup when parent exits
        close(fd1);
        close(fd2);
        
        // Terminate child process if it's still running
        kill(pid, SIGTERM);
        wait(NULL);

        // Remove pipes
        unlink(FIFO1);
        unlink(FIFO2);
        printf("Server exited.\n");
        exit(0);

    } else {
        // Child process: Blocks on the incoming FIFO (FIFO2) and renders received data
        while (1) {
            ssize_t n = read(fd2, buf, BUFSIZ);
            if (n <= 0) {
                break; // EOF or error
            }

            // Check for exit condition from Client
            if (strcmp(buf, ".\n") == 0) {
                printf("Client disconnected.\n");
                kill(getppid(), SIGTERM); // Notify parent to exit
                break;
            }

            // Print received message
            printf("Client: %s", buf);
            fflush(stdout);
        }

        close(fd1);
        close(fd2);
        exit(0);
    }

    return 0;
}
