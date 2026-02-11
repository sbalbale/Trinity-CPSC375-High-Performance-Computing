#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#define SHM_KEY_PATH "monte_master.c"
#define SHM_KEY_ID 65
#define SEM_KEY_ID 66
#define MSG_KEY_ID 67

// Message structure for message queue
struct msg_buf
{
    long mtype;       // Message type (must be > 0)
    long long tosses; // Payload: number of tosses for this task
};

// Global variables for signal handling and cleanup
int m_pid_arr[100]; // Array to keep track of worker PIDs
int num_workers_spawned = 0;
int shmid = -1, semid = -1, msgid = -1; // IPC Identifiers
volatile sig_atomic_t paused = 0;       // Flag to pause operations
volatile sig_atomic_t terminate = 0;    // Flag to terminate operations

// Cleanup function to remove IPC resources and terminate workers
void cleanup()
{
    int i;
    // Send SIGINT to all spawned workers
    for (i = 0; i < num_workers_spawned; i++)
    {
        kill(m_pid_arr[i], SIGINT);
    }
    // Wait for all child processes to exit
    while (wait(NULL) > 0)
        ;

    // Mark Shared Memory for destruction
    if (shmid != -1)
        shmctl(shmid, IPC_RMID, NULL);
    // Remove Semaphore set
    if (semid != -1)
        semctl(semid, 0, IPC_RMID);
    // Remove Message Queue
    if (msgid != -1)
        msgctl(msgid, IPC_RMID, NULL);
}

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\nMaster received SIGINT. cleaning up...\n");
        terminate = 1;
        cleanup();
        exit(0);
    }
    else if (signo == SIGUSR1)
    {
        printf("Master received SIGUSR1. Pausing...\n");
        paused = 1;
    }
    else if (signo == SIGUSR2)
    {
        printf("Master received SIGUSR2. Resuming...\n");
        paused = 0;
    }
}

int main(int argc, char *argv[])
{
    int M = 1;
    long long N = 1000000;
    long long C = 100000;
    int S = 1;

    // Parse arguments
    // Note: manual parsing or getopt. Simple manual parsing for args like -M 2 -N ...
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'M':
                if (i + 1 < argc)
                    M = atoi(argv[++i]);
                else
                {
                    fprintf(stderr, "Missing arg for -M\n");
                    exit(1);
                }
                break;
            case 'N':
                if (i + 1 < argc)
                    N = atoll(argv[++i]);
                else
                {
                    fprintf(stderr, "Missing arg for -N\n");
                    exit(1);
                }
                break;
            case 'C':
                if (i + 1 < argc)
                    C = atoll(argv[++i]);
                else
                {
                    fprintf(stderr, "Missing arg for -C\n");
                    exit(1);
                }
                break;
            case 'S':
                if (i + 1 < argc)
                    S = atoi(argv[++i]);
                else
                {
                    fprintf(stderr, "Missing arg for -S\n");
                    exit(1);
                }
                break;
            }
        }
    }

    printf("M=%d, N=%lld, C=%lld, S=%d\n", M, N, C, S);

    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);

    // Setup Shared Memory
    key_t key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    shmid = shmget(key, sizeof(long long), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    // Initialize Shared Memory (Global Count) to 0
    long long *global_count = (long long *)shmat(shmid, NULL, 0);
    *global_count = 0;
    shmdt(global_count); // Detach for now, will re-attach at end

    // Setup Semaphore
    key_t sem_key = ftok(SHM_KEY_PATH, SEM_KEY_ID);
    semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid < 0)
    {
        perror("semget");
        cleanup();
        exit(1);
    }
    // Init semaphore value to 1 (Mutex)
    semctl(semid, 0, SETVAL, 1);

    // Setup Message Queue
    key_t msg_key = ftok(SHM_KEY_PATH, MSG_KEY_ID);
    msgid = msgget(msg_key, IPC_CREAT | 0666);
    if (msgid < 0)
    {
        perror("msgget");
        cleanup();
        exit(1);
    }

    for (int i = 0; i < M; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // CHILD PROCESS (Worker)
            char s_str[20];
            sprintf(s_str, "%d", S + i); // Calculate unique seed for worker
            // Execute worker program
            execl("./monte_worker", "./monte_worker", s_str, NULL);
            perror("execl failed");
            exit(1);
        }
        else if (pid > 0)
        {
            // PARENT PROCESS
            m_pid_arr[num_workers_spawned++] = pid;
        }
        else
        {
            perror("fork");
        }
    }

    time_t start = time(NULL);

    struct msg_buf msg;
    msg.mtype = 1;

    long long remaining = N;

    while (remaining > 0 && !terminate)
    {
        // Handle PAUSE signal (SIGUSR1)
        while (paused && !terminate)
        {
            sleep(1); // Wait until resumed
        }
        if (terminate)
            break;

        // Calculate chunk size (C or whatever is left)
        long long current_chunk = (remaining > C) ? C : remaining;
        msg.tosses = current_chunk;

        // Send task to Message Queue
        if (msgsnd(msgid, &msg, sizeof(long long), 0) == -1)
        {
            if (errno == EINTR)
            {
                // Interrupted by signal (e.g. pause), just retry the loop
                continue;
            }
            perror("msgsnd");
            break;
        }

        remaining -= current_chunk;
    }

    // Send Empty Messages (size 0) to signal workers to exit
    for (int i = 0; i < M; i++)
    {
        msg.tosses = 0; // 0 indicates termination to worker
        msgsnd(msgid, &msg, sizeof(long long), 0);
    }

    // Wait for all workers to finish
    while (wait(NULL) > 0)
        ;

    time_t end = time(NULL);

    // Read Final Result from Shared Memory
    global_count = (long long *)shmat(shmid, NULL, 0);
    double pi_estimate = 4.0 * (*global_count) / ((double)N);
    printf("Pi estimate: %f\n", pi_estimate);
    printf("Elapsed time = %ld seconds\n", end - start);

    shmdt(global_count);
    cleanup(); // Final cleanup of IPC

    return 0;
}
