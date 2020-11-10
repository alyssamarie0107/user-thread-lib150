#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/*the internal counter of the semaphore */
	size_t internal_counter;
	/* queue to store threads that get blocked when trying to call down on a busy semaphore */
	/* the collection of threads in queue are those that are waiting for some resource to become available */
	queue_t blocked_queue; /* queue of blocked threads */
};

static struct uthread_tcb *blocked_thread; 

/* this function creates a semaphore */
sem_t sem_create(size_t count)
{
	printf("start sem_create()\n");
	/* allocate space on heap for the new semaphore */
	sem_t new_semaphore = (struct semaphore*)malloc(sizeof(struct semaphore));

	/* initialize the internal counter to count */
	new_semaphore->internal_counter = count;

	/* create the block_threads queue */
	new_semaphore->blocked_queue = queue_create();

	/* if remaphore is NULL (not allocated), return NULL; otherwise return the semaphore pointer */
	if(new_semaphore == NULL) {
		return NULL;
	}
	else {
		return new_semaphore;
	}
	printf("end sem_create()\n");
}

/* this function deallocates a semaphore */
int sem_destroy(sem_t sem)
{
	printf("start sem_destroy()\n");
	/* if sem is NULL or there are still threads in block_threads waiting on it, return -1*/
	if(sem == NULL || queue_length(sem->blocked_queue) != 0) {
		return -1;
	}
	else {
		/* deallocate memory for sem */
		free(sem);
	}
	printf("end sem_create()\n");
	return 0;
}

/* cause the count to decrement */
int sem_down(sem_t sem)
{
	printf("start sem_down()\n");
	/* assign the current running thread from uthread.c to blocked thread */
	blocked_thread = uthread_current();

	/* return -1 if semaphore is not allocated */
	if(sem == NULL) {
		return -1;
	}

	/* if current thread tries to call down() on a 0 semaphore, it is blocked and put in the blocked queue  */
	if(sem->internal_counter == 0){
		printf("enqueuing curr thread to blocked queue bc it called down() on a 0 semaphore\n");
		queue_enqueue(sem->blocked_queue, blocked_thread);

		/* block the thread */
		printf("calling uthread_block()\n");
		uthread_block();
	}
	/*otherwise, decrement the internal counter*/
	else { 
		printf("decrementing sem counter\n");
		sem->internal_counter--;
	}
	printf("end of sem_down()\n");
	return 0;
}

/* 
 * cause the count to increment 
 * if after the increment, count is <= 0, then there is still some blocked processes in the queue 
 * thus, one of them becomes dequeued and unblocked making it able to run again
 */
int sem_up(sem_t sem)
{
	printf("start sem_up()\n");
	/* return -1 if semaphore is not allocated */
	if(sem == NULL) {
		return -1;
	}

	/* if blocked queue is not empty, release resource and increment internal count */
	if(queue_length(sem->blocked_queue) > 0) {
		sem->internal_counter++;
		/* unblock the thread */
		printf("dequeuing blocked thread fro blocked queue ()\n");
		queue_dequeue(sem->blocked_queue, (void **)&blocked_thread);
		printf("calling uthread_unblocked()\n");
		uthread_unblock(blocked_thread);
	}
	/* case in which there are no waiting threads in blocked queue */
	else {
		printf("incrementing sem counter\n");
		sem->internal_counter++;
	}
	printf("end sem_up()\n");
	return 0;
}
