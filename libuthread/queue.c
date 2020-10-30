#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {

	struct Node {
		int data;
		struct Node *next;

	} *head, *tail, *current; /* pointer to the head, tail and current element of the queue */

	
	int q_size;
};

queue_t queue_create(void)
{
	queue_t new_q = malloc(sizeof(queue_t));

	/* initialize the queue */
	new_q->current = new_q->tail = new_q->head = malloc(sizeof(struct Node));
	new_q->q_size = 0;

	return new_q;

}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
}

int queue_enqueue(queue_t queue, int data)
{	
	if(queue != NULL) {
		queue->tail = queue->tail->next = malloc(sizeof(struct Node));
		queue->q_size++;
		queue->tail->data = data;
		queue->tail->next = NULL;
		return 0;
	}
	
	return -1;
	
}


int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	return queue.q_size;
}