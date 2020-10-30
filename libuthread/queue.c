#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {
	/* TODO Phase 1 */
	struct node {
		void* data;
		node *next;
        
		/* constructor for a node with data and pointer to next node */
		node(void* new_data, node *next_val=NULL) {
			data = new_data;
			next = next_val;
		}

		/* constructor for the pointer to next node */
		node(node *next_val = NULL) {
			next = next_val;
		}
	}; node

	/* pointer to the head, tail and current element of the queue */
	node *head;
	node *tail;
	node *current;
	int q_size;
};

queue_t queue_create(void)
{
	/* TODO Phase 1 */
	queue_t new_q;

	/* initialize the queue */
	new_q.current = new_q.tail = new_q.head = malloc(sizeof(node));
	new_q.q_size = 0;


}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
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
	/* TODO Phase 1 */
}