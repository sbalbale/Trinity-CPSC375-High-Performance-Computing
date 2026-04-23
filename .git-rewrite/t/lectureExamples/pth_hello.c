/*    This program illustrates basic use of pthreads:  create some threads,
 *    each of which prints a message.
 *
 * Compile:  gcc -o pth_hello pth_hello.c -lpthread
 * Usage:    ./pth_hello <thread_count>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 

/* Global variable:  accessible to all threads */
int thread_count;  

/*
 * thread function
 */
void *hello(void *rank) {
   long my_rank = (long) rank;  

   printf("Hello from thread %ld of %d\n", my_rank, thread_count);

   return NULL;
}  /* hello */


int main(int argc, char *argv[]) {
   long       tid;  
   pthread_t *thread; 

   /* Get number of threads from command line */
   thread_count = atoi(argv[1]);  
   thread = (pthread_t *) malloc (thread_count*sizeof(pthread_t)); 

   /* create threads */
   for (tid = 0; tid < thread_count; tid++)  
      pthread_create(&thread[tid], NULL, hello, (void *) tid);  

   printf("Hello from the main thread\n");

   /* wait for threads */
   for (tid = 0; tid < thread_count; tid++) 
      pthread_join(thread[tid], NULL); 

   free(thread);

   return 0;
}  
