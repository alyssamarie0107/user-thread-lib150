#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

/* the queue stores the front node of linked list and the rear node of linked list */
struct queue {
	int queue_size; /* keeps track of the size of the queue */
	/* a linked list node that stores queue entry */
	struct node {
		void *data; /* any data type can be stored in this node */
		struct node *next;
	} *front, *rear, *current ; 
};

queue_t queue_create(void)
{	
	queue_t new_queue = (queue_t)malloc(sizeof(struct queue)); /* allocating memory for a new queue*/
	new_queue->front = new_queue->rear = NULL; /* want the new queue to be empty, thus initializing front and rear ptrs to NULL */
	new_queue->queue_size = 0;

	return new_queue;
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

	/* if new node is still NULL after attempt to allocate memory and insert data, return -1 */
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
		return 0;
    } 
	/* if queue is not empty */	
	else {
		/* the rear node's next will be set to new_node's address & rear node point to the new_node that has been created*/
		queue->rear = queue->rear->next = new_node; 
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

int queue_destroy(queue_t queue)
{	
	/* return -1 if queue is empty */
	if(queue->front == NULL && queue->rear == NULL) {
		return -1;
	}

	while(queue->front != NULL) {
		queue->current = queue->front;
		queue->front = queue->front->next;
		queue->queue_size--;
		free(queue->current);
	}

	/* printing the queue's content */
	struct node *temp = queue->front;
	printf("queue content: ");
	for(int i = 0; i < queue->queue_size; i++) {
		printf("%d ",*(int *)temp->data);
		temp = temp->next;
	}
	printf("\n");
    
	/*if after destroying everything queue is still not empty, return -1 */
	if(queue->front != NULL || queue->queue_size != 0) {
		return -1;
	}
	return 0;
} 

int queue_dequeue(queue_t queue, void **data)
{
	queue->current = queue->front;

	/* check if queue is empty */
	if(queue->front == NULL && queue->rear == NULL) {
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
	*data = queue->current->data;
	free(queue->current);
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
	struct node *temp = queue->front;
    struct node *previous = NULL;

    if(queue->front == NULL) {
        return -1;
    }

    if(queue->front->data == data) {
        queue->front = queue->front->next;
		queue->queue_size--;
		
		/* printing the queue's content */
		struct node *temp = queue->front;
		printf("queue content after deleting front: ");
		for(int i = 0; i < queue->queue_size; i++) {
			printf("%d ",*(int *)temp->data);
			temp = temp->next;
		}
		printf("\n");
        return 0;
    }

    while(temp != NULL && temp->data != data) {
        if(temp == queue->rear) {
            return -1;
        }
        previous = temp;
        temp = temp->next;
    }

    if(queue->rear->data == data) {
        previous->next = NULL;
		queue->rear = previous;
		queue->queue_size--;
        free(temp);
		
		/* printing the queue's content */
		struct node *temp = queue->front;
		printf("queue content after deleting end: ");
		for(int i = 0; i < queue->queue_size; i++) {
			printf("%d ",*(int *)temp->data);
			temp = temp->next;
		}
		printf("\n");
    }
    else {
        previous->next = temp->next;
		queue->queue_size--;
        free(temp);
		
		/* printing the queue's content */
		struct node *temp = queue->front;
		printf("queue content after deleting an element: ");
		for(int i = 0; i < queue->queue_size; i++) {
			printf("%d ",*(int *)temp->data);
			temp = temp->next;
		}
		printf("\n");
    }
    return 0;
}


/*
 * This function iterates through the items in the queue @queue, from the oldest
 * item to the newest item, and calls the given callback function @func on each
 * item. The callback function receives the current data item as parameter.
*/
int queue_iterate(queue_t queue, queue_func_t func) {
	/* return: -1 if @queue or @func are NULL, 0 otherwise. */
	if((queue->front == NULL && queue->rear == NULL) || func == NULL) {
		return -1;
	}
	else {
		queue->current = queue->front;
		printf("queue content: ");
		/* iterate throught the queue and apply th*/
		for(int i = 0; i < queue->queue_size; i++) {
			func(queue->current->data);
			printf("%d ",*(int *)queue->current->data);
			queue->current = queue->current->next;
		}
		printf("\n");
	}
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->queue_size;
}