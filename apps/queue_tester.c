#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"



#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}


/* callback function that increments items */
static void inc_item(void *data)
{
    int *a = (int*)data;
    *a += 1;
}
/* callback function that prints to console each element of queue*/
static void print_item(void *data)
{
    int *a = (int*)data;
    printf("%d ",*a);
}



/* test queue_destroy */
void test_destroy(void) {
	queue_t q;
	int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5, data6 = 6;
	q = queue_create();

	fprintf(stderr, "*** TEST queue destroy if queue is empty ***\n");
	TEST_ASSERT(queue_destroy(q) == -1);

	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_enqueue(q, &data4);
	queue_enqueue(q, &data5);
	queue_enqueue(q, &data6);

	fprintf(stderr, "*** TEST queue destroy queue was not empty ***\n");
	TEST_ASSERT(queue_destroy(q) == 0);
	TEST_ASSERT(queue_length(q) == 0);
	queue_destroy(q);
		
}
/* test queue_delete() functions/returns */
void test_delete_ints(void) {
	queue_t q;
	int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5, data6 = 6;
	int empty_data;

	q = queue_create();

	fprintf(stderr, "*** TEST delete empty queue ***\n");
	TEST_ASSERT(queue_delete(q, &data1) == -1);

	fprintf(stderr, "*** TEST delete empty data ***\n");
	TEST_ASSERT(queue_delete(q, &empty_data) == -1);

	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_enqueue(q, &data4);
	queue_enqueue(q, &data5);
	
	fprintf(stderr, "*** TEST delete element found at head of queue ***\n");
	TEST_ASSERT(queue_delete(q, &data1) == 0);

	queue_enqueue(q, &data6);

	fprintf(stderr, "*** TEST delete element found at tail of queue ***\n");

	TEST_ASSERT(queue_delete(q, &data6) == 0);


	fprintf(stderr, "*** TEST delete element found at location other than head or tail of queue ***\n");
	TEST_ASSERT(queue_delete(q, &data3) == 0);
	//TEST_ASSERT(queue_delete(q, &data6) == 0);


	fprintf(stderr, "*** TEST delete element not found in queue (3)***\n");
	TEST_ASSERT(queue_delete(q, &data3) == -1);
	queue_destroy(q);

}
/* test queue_iterate() functions/returns */
void test_iterator(void)
{	
	queue_t q;

    fprintf(stderr, "*** TEST iterator ***\n");
    int data[] = {1, 2, 3, 4, 6, 7, 8, 9, 10};
    int i;
    int size_of_data = sizeof(data)/sizeof(data[0]);

    /* Initialize the queue and enqueue items */
    q = queue_create();
     for (i = 0; i < size_of_data; i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '5' */
    queue_iterate(q, inc_item);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
	queue_destroy(q);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	queue_t q;
	int data = 3, *ptr;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
	queue_destroy(q);
}

/* this functions tests the queue by adding new elements to it, dequeing and deleting operations
 * in a mixed fashion. Instead of using TEST_ASSERT to show the results, we used print statements */
void test_mixed_operations(void) {
	queue_t q;
	printf("Test mixed enqueue/dequeue/delete operations: \n");
	int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5, data6 = 6, data7 = 7, data8 = 8;
	int *ptr;
	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_enqueue(q, &data4);
	queue_enqueue(q, &data5);
	queue_enqueue(q, &data6);

	printf("After enqueuing, the queue should contain: 1,2,3,4,5,6: \n");
	queue_iterate(q, print_item);
	printf("\n");

	queue_dequeue(q, (void **)&ptr);
	printf("After dequeue, the queue should contain: 2,3,4,5,6: \n");
	queue_iterate(q, print_item);
	printf("\n");

	queue_delete(q,&data2);
	printf("After deleting the head (3), the queue should contain: 3,4,5,6: \n");
	queue_iterate(q, print_item);
	printf("\n");

	queue_delete(q,&data6);
	printf("After deleting the tail (6), the queue should contain: 3,4,5: \n");
	queue_iterate(q, print_item);
	printf("\n");

	queue_delete(q,&data4);
	printf("After deleting data that is not tail or head (4), the queue should contain: 3,4,5: \n");
	queue_iterate(q, print_item);
	printf("\n");

	queue_enqueue(q, &data7);
	queue_enqueue(q, &data8);
	printf("After inserting again 7,8 ,the queue should contain: 3,5,7,8: \n");
	queue_iterate(q, print_item);
	printf("\n");

	queue_destroy(q);
}

int main(void)
{
	test_create();
	printf("\n*****************NEW TEST**************************************************************************************************\n");
	test_queue_simple();
	printf("\n*****************NEW TEST**************************************************************************************************\n");
	test_delete_ints();
	printf("\n*****************NEW TEST**************************************************************************************************\n");
	test_destroy();
	printf("\n*****************NEW TEST**************************************************************************************************\n");
    test_iterator();
	printf("\n*****************NEW TEST**************************************************************************************************\n"); 
	test_mixed_operations();


	return 0;
}
