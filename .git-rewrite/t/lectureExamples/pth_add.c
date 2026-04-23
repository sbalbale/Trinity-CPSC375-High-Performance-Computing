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

/*
 * thread function
 */
void *add(void *pkg) {
   PKG *mypkg = (PKG *) pkg;
   int i;

   mypkg->sum = 0;
   for (i = (mypkg->tid)*(mypkg->nInt)+1; i <= (mypkg->tid + 1)*(mypkg->nInt); i++)
      mypkg->sum += i;

   printf("Hello from thread %ld sum = %d\n", mypkg->tid, mypkg->sum);

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
   for (tid = 0; tid < thread_count; tid++) {
      package[tid].tid = tid;
      package[tid].nInt = N/thread_count; // assume N is divisible by thread_count
      pthread_create(&thread[tid], NULL, add, (void *) &package[tid]);  
   }

   /* wait for threads */
   for (tid = 0; tid < thread_count; tid++) {
      pthread_join(thread[tid], NULL); 
      total += package[tid].sum;
   }

   printf("Hello from the main thread: total = %d\n", total);

   free(thread);

   return 0;
}  
