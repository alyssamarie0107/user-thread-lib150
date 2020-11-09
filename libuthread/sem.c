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
	/* TODO Phase 3 */
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
}

