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

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	int data2 = 5;
    int size, ret;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);
	ret = queue_enqueue(q, &data);
    fprintf(stdout, " ret = %d\n", ret);
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);
	ret = queue_enqueue(q, &data2);
    fprintf(stdout, " ret = %d\n", ret);
    //fprintf(stdout, "enqued = %d", q->tail->data);
	queue_dequeue(q, (void**)&ptr);
	size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);
	ret = queue_destroy(q);
	fprintf(stdout, " ret = %d\n", ret);
	size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);
	TEST_ASSERT(ptr == &data); 

	/* int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data); */
}
 
int main(void)
{
	test_create();
	test_queue_simple();

	return 0;
}
