#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/*the internal counter of the semaphore */
	size_t internal_counter;
	/* queue to store threads that get blocked when trying to call down on a busy semaphore */
	queue_t blocked_threads;
};

sem_t sem_create(size_t count)
{
	/* allocate space on heap for the new semaphore */
	sem_t new_semaphore = (sem_t*)malloc(sizeof(sem_t));

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
	if(sem->internal_counter == 0){
		
		queue_enqueue(sem->blocked_threads, blocked_thread);
		/*we would have to take the blocked_thread from some queue - maybe running queue */

		/* block the thread */
		uthread_block();
	}
	else { /*otherwise, decrement the internal counter*/
		sem->internal_counter--;
	}
	return 0;
}

int sem_up(sem_t sem)
{
	/* return -1 if semaphore is not allocated */
	if(sem == NULL) {
		return -1;
	}

	/* if the waiting list is not empty, release resource and increment internal count */
	if(queue_length(sem->blocked_threads) > 0) {
		sem->internal_counter++;
		uthread_unblock();
		
	}
	return 0;
}

