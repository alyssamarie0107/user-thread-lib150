#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	size_t internal_counter; /* the internal counter of the semaphore */
	/* queue to store threads that get blocked when trying to call down on a busy semaphore */
	/* the collection of threads in queue are those that are waiting for some resource to become available */
	queue_t blocked_queue;
};

/* global structure pointer */
static struct uthread_tcb *blocked_thread; 

/* this function creates a semaphore */
sem_t sem_create(size_t count)
{
	/* printf("start sem_create()\n"); */
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
	/* printf("end sem_create()\n"); */
}

/* this function deallocates a semaphore */
int sem_destroy(sem_t sem)
{
	/* printf("start sem_destroy()\n"); */
	
	/* if sem is NULL or there are still threads in block_threads waiting on it, return -1 */
	if(sem == NULL || queue_length(sem->blocked_queue) != 0) {
		return -1;
	}
	else {
		/* deallocate memory for sem */
		free(sem);
	}
	/* printf("end sem_create()\n"); */
	return 0;
}

/* cause the count to decrement */
int sem_down(sem_t sem)
{
	/* printf("start sem_down()\n"); */

	/* return -1 if semaphore is not allocated */
	if(sem == NULL) {
		return -1;
	}

	/* CRITICAL SECTION : modifying global struct ptr -> blocked_thread */
	preempt_disable(); /* disable forced interrupts */

	blocked_thread = uthread_current(); /* assign the current running thread from uthread.c to blocked thread */

	/* END OF CRITICAL SECTION */
	preempt_enable(); /* done modifying global variables, thus enable preemption */

	/* if current thread tries to call down() on a 0 or negative semaphore, it is blocked and put in the blocked queue */
	if(sem->internal_counter <= 0){
		sem->internal_counter--;
		/* printf("enqueuing curr thread to blocked queue bc it called down() on a 0 semaphore\n"); */
		queue_enqueue(sem->blocked_queue, blocked_thread);

		/* block the thread */
		/* printf("calling uthread_block()\n"); */
		uthread_block();
	}
	/* otherwise, decrement the internal counter*/
	else { 
		/* printf("decrementing sem counter\n"); */
		sem->internal_counter--;
	}
	/* printf("end of sem_down()\n"); */
	return 0;
}

int sem_up(sem_t sem)
{
	/* printf("start sem_up()\n"); */
	/* return -1 if semaphore is not allocated */
	if(sem == NULL) {
		return -1;
	}

	/* if blocked queue is not empty, release resource and increment internal count */
	if(queue_length(sem->blocked_queue) != 0) {
		sem->internal_counter++;
		/* printf("dequeuing blocked thread from blocked queue ()\n"); */
		queue_dequeue(sem->blocked_queue, (void **)&blocked_thread);

		/* printf("calling uthread_unblocked()\n"); */
		/* unblock the thread */
		uthread_unblock(blocked_thread);
	}
	/* case in which there are no waiting threads in blocked queue */
	else {
		/* printf("incrementing sem counter\n"); */
		sem->internal_counter++;
	}
	/* printf("end sem_up()\n"); */
	return 0;
}
