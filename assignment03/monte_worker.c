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
#include <errno.h>

// IPC Definitions - Must match master
#define SHM_KEY_PATH "monte_master.c"
#define SHM_KEY_ID 65
#define SEM_KEY_ID 66
#define MSG_KEY_ID 67

// Message Buffer structure
struct msg_buf
{
    long mtype;
    long long tosses;
};

// Global flags for signal handling
volatile sig_atomic_t paused = 0;
volatile sig_atomic_t terminate = 0;

// Signal Handler
void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        terminate = 1; // Mark for termination
    }
    else if (signo == SIGUSR1)
    {
        paused = 1; // Pause signal received
    }
    else if (signo == SIGUSR2)
    {
        paused = 0; // Resume signal received
    }
}

int main(int argc, char *argv[])
{
    int seed = 1;
    if (argc > 1)
    {
        seed = atoi(argv[1]);
    }
    srand(seed);

    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);

    // --- SETUP IPC CONNECTIONS ---

    // 1. Get Shared Memory
    key_t key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    if (key == -1)
    {
        perror("worker ftok");
        exit(1);
    }
    int shmid = shmget(key, sizeof(long long), 0666);
    if (shmid < 0)
    {
        if (errno == ENOENT)
        {
            fprintf(stderr, "worker: Shared memory not found. Master process may have finished or hasn't started.\n");
        }
        else
        {
            perror("worker shmget");
        }
        exit(1);
    }

    // Attach Shared Memory
    long long *global_count = (long long *)shmat(shmid, NULL, 0);

    // 2. Get Semaphore
    key_t sem_key = ftok(SHM_KEY_PATH, SEM_KEY_ID);
    if (sem_key == -1)
    {
        perror("worker ftok sem");
        exit(1);
    }
    int semid = semget(sem_key, 1, 0666);
    if (semid < 0)
    {
        perror("worker semget");
        exit(1);
    }

    // 3. Get Message Queue
    key_t msg_key = ftok(SHM_KEY_PATH, MSG_KEY_ID);
    if (msg_key == -1)
    {
        perror("worker ftok msg");
        exit(1);
    }
    int msgid = msgget(msg_key, 0666);
    if (msgid < 0)
        // Semaphore Operations
        struct sembuf p_op = {0, -1, 0}; // Wait / Lock
    struct sembuf v_op = {0, 1, 0};      // Signal / Unlock

    // --- MAIN LOOP ---
    while (!terminate)
    {
        // Handle Pausing
        while (paused && !terminate)
        {
            sleep(1);
        }
        if (terminate)
            break;

        // Receive task from Queue (Blocking)
        if (msgrcv(msgid, &msg, sizeof(long long), 1, 0) == -1)
        {
            if (errno == EIDRM || errno == EINVAL)
            {
                // Queue removed or invalid - Master likely finished
                break;
            }
            if (errno == EINTR)
            {
                // Interrupted (e.g. by signal), retry
                continue;
            }
            if (terminate)
                break;
            perror("msgrcv");
            exit(1);
        }

        if (msg.tosses == 0)
        {
            // Termination signal from master
            break;
        }

        // Perform Calculation
        long long local_in_circle = 0;
        double x, y, dist;
        for (long long i = 0; i < msg.tosses; i++)
        {
            x = (double)rand() / RAND_MAX * 2.0 - 1.0;
            y = (double)rand() / RAND_MAX * 2.0 - 1.0;
            dist = x * x + y * y;
            if (dist <= 1.0)
                local_in_circle++;
        }

        // Critical Section: Update Global Count
        semop(semid, &p_op, 1); // Lock
        *global_count += local_in_circle;
        semop(semid, &v_op, 1); // Unlock++;
    }

    // Update Global
    semop(semid, &p_op, 1);
    *global_count += local_in_circle;
    semop(semid, &v_op, 1);
}

shmdt(global_count);
return 0;
}
