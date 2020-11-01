#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* a linked list node that stores queue entry */
struct node {
	void *data; /* any data type can be stored in this node */
	struct node *next;
};

/* the queue stores the front node of linked list and the rear node of linked list */
struct queue {
	int queue_size; /* keeps track of the size of the queue */
	struct node *front, *rear;
};


queue_t queue_create(void)
{
	queue_t new_queue = (struct queue*)malloc(sizeof(struct queue)); /* allocating memory for a new queue*/
	new_queue->front = new_queue->rear = NULL; /* want the new queue to be empty, thus initializing front and rear ptrs to NULL */
	new_queue->queue_size = 0;

	return new_queue;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
}

int queue_enqueue(queue_t queue, void *data)
{
	/* return: -1 if @queue or @data are NULL, or in case of memory allocation error */
	if (data == NULL) {
		return -1;
	}

	/* create new node */
	struct node *new_node = (struct node*)malloc(sizeof(struct node)); /* allocate memory using malloc () */
	new_node->data = data;
	new_node->next = NULL;

	if (new_node == NULL) {
		return -1;
	}

    // If queue is empty, set the front and rear node of queue to the new node
    if (queue->front == NULL && queue->rear == NULL) { 
        queue->front = queue->rear = new_node;

		queue->queue_size++;
		/* printing the queue's content */
		struct node *temp = queue->front;
		printf("queue content: ");
		for(int i = 0; i < queue->queue_size; i++) {
			printf("%d ",*(int *)temp->data);
			temp = temp->next;
		}
		printf("\n");
		return -1;
    } 
	/* if queue is not empty */	
	else {
		queue->rear->next = new_node; /* the rear node's next will be set to new_node's address */
		queue->rear = new_node; /* let rear node point to the new_node that has been created */
		//printf("queue_rear: %d\n", *(int *)queue->rear->data);

		queue->queue_size++;

		/* printing the queue's content */
		struct node *temp = queue->front;
		printf("queue content: ");
		for(int i = 0; i < queue->queue_size; i++) {
			printf("%d ",*(int *)temp->data);
			temp = temp->next;
		}
		printf("\n");
	}
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	struct node *remove_front = queue->front;

	/* check if queue is empty */
	if(queue->front == NULL) {
		return -1;
	}
	/* check if the queue only has one data entry */
	if (queue->front == queue->rear) {
		queue->front = queue->rear = NULL;
	}
	/* if queue is not empty, make queue's front ptr point to the next node */
	else {
		queue->front = queue->front->next;
	}
	/* remove the oldest item of queue @queue and assign this item (the value of a pointer) to @data. */
	*data = remove_front->data;
	free(remove_front);
	queue->queue_size--;

	/* printing the queue's content */
	struct node *temp = queue->front;
	printf("queue content: ");
	for(int i = 0; i < queue->queue_size; i++) {
		printf("%d ",*(int *)temp->data);
		temp = temp->next;
	}
	printf("\n");
	
	return 0;
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
	return queue->queue_size;
}