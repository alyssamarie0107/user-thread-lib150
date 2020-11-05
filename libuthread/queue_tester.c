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
/* test queue_destroy */
void test_destroy(void) {
	int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5, data6 = 6;
	queue_t q;
	q = queue_create();

	fprintf(stderr, "*** TEST queue destroy if queue is empty***\n");
	TEST_ASSERT(queue_destroy(q) == -1);

	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_enqueue(q, &data4);
	queue_enqueue(q, &data5);
	queue_enqueue(q, &data6);

	fprintf(stderr, "*** TEST queue destroy if queue is has elements ***\n");
	TEST_ASSERT(queue_destroy(q) == 0);
	TEST_ASSERT(queue_length(q) == 0);
		
}
/* test queue_delete() functions/returns */
void test_delete_ints(void) {
	int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5, data6 = 6;
	int empty_data;

	queue_t q;

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

	fprintf(stderr, "*** TEST delete element not found in queue ***\n");
	TEST_ASSERT(queue_delete(q, &data3) == -1);

}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
	

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

	return 0;
}