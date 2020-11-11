#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

static struct uthread_tcb *unblock_thread;
struct semaphore {
	/*the internal counter of the semaphore */
	size_t internal_counter;
	/* queue to store threads that get blocked when trying to call down on a busy semaphore */
	queue_t blocked_threads;
};

sem_t sem_create(size_t count)
{
	/* allocate space on heap for the new semaphore */
	sem_t new_semaphore = (struct semaphore*)malloc(sizeof(sem_t));

	/* initialize the internal counter to count */
	new_semaphore->internal_counter = count;
	/* create the block_threads queue */
	new_semaphore->blocked_threads = queue_create();

	/* if remaphore is NULL (not allocated), return NULL; otherwise return the semaphore pointer */
	if(new_semaphore == NULL) {
		return NULL;
	}
	else {
		return new_semaphore;
	}

}

int sem_destroy(sem_t sem)
{
	/* if sem is NULL or there are still threads in block_threads waiting on it, return -1*/
	if(sem == NULL || queue_length(sem->blocked_threads) != 0) {
		return -1;
	}

	/* deallocate memory for sem */
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	/* return -1 if semaphore is not allocated */
	if(sem == NULL) {
		return -1;
	}

	/* if a thread tries to call down on a 0 semaphore, put it in the block_threads queue
	 * and block it */
	while(sem->internal_counter == 0){

		/* enque the currently running thread on the waiting list*/
		queue_enqueue(sem->blocked_threads, uthread_current());

		//unblock_thread = uthread_current();
		//printf("blocked thread is: %d", unblock_thread->tcb_id);

		/* block the thread */
		uthread_block();
	}
	//else { /*otherwise, decrement the internal counter*/
		sem->internal_counter--;
	//}
	return 0;
}

int sem_up(sem_t sem)
{
	/* return -1 if semaphore is not allocated */
	if(sem == NULL) {
		return -1;
	}

	/* if the waiting list is not empty, release resource and increment internal count */
	
	sem->internal_counter++;

	if(queue_length(sem->blocked_threads) > 0) {
		queue_dequeue(sem->blocked_threads, (void **)&unblock_thread);
		uthread_unblock(unblock_thread);
		
	}
	return 0;
}

