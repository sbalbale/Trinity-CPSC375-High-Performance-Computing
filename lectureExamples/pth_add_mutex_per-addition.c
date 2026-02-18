/*    This program computes the sum of N integers using multiples threads.
 *
 * Compile:  gcc -o pth_add pth_add.c -lpthread
 * Usage:    ./pth_add <N> <thread_count>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 

typedef struct {
   long tid;
   int nInt;
   int sum;
} PKG;

int sum = 0;
pthread_mutex_t mutex;

/*
 * thread function
 */
void *add(void *pkg) {
   PKG *mypkg = (PKG *) pkg;
   int i;

   int start = (mypkg->tid)*(mypkg->nInt)+1;
   int end = (mypkg->tid + 1)*(mypkg->nInt);

   for (i = start; i <= end; i++) {
        pthread_mutex_lock(&mutex);
        sum += i;
        pthread_mutex_unlock(&mutex);
   }

   printf("Hello from thread %ld\n", mypkg->tid);

   return NULL;
}  /* hello */


int main(int argc, char *argv[]) {
   int thread_count;
   long tid;  
   int N, total = 0;
   pthread_t *thread; 
   PKG *package;

   /* Get number of threads from command line */
   N = atoi(argv[1]);  
   thread_count = atoi(argv[2]);  
   thread = (pthread_t *) malloc (thread_count*sizeof(pthread_t)); 
   package = (PKG *) malloc(thread_count*sizeof(PKG));

   /* create threads */
   pthread_mutex_init(&mutex, NULL);
   for (tid = 0; tid < thread_count; tid++) {
      package[tid].tid = tid;
      package[tid].nInt = N/thread_count; // assume N is divisible by thread_count
      pthread_create(&thread[tid], NULL, add, (void *) &package[tid]);  
   }

   /* wait for threads */
   for (tid = 0; tid < thread_count; tid++) {
      pthread_join(thread[tid], NULL); 
   }

   printf("Hello from the main thread: sum = %d\n", sum);
   pthread_mutex_destroy(&mutex);

   free(thread);

   return 0;
}  
