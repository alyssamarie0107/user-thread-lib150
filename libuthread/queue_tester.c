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
	//int *ptr;
    int data1 = 1; //, data2 = 2, data3 = 3, data4 = 4, data5 = 5, data6 = 6, data7 = 7;
    //int size,ret;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	TEST_ASSERT(queue_delete(q, &data1) == -1);
    
	/*queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_enqueue(q, &data4);
	queue_enqueue(q, &data5);
	queue_enqueue(q, &data6);
	size = queue_length(q);
	fprintf(stdout, "(size = %d\n", size);
	queue_delete(q, &data2);
	size = queue_length(q);
	fprintf(stdout, "(after delete 3) size = %d\n", size);
	queue_delete(q, &data6);
	size = queue_length(q);
	fprintf(stdout, "(after delete6) size = %d\n", size);
	ret = queue_enqueue(q, &data7);
    fprintf(stdout, " ret = %d\n", ret); 
	queue_dequeue(q, (void**)&ptr);
	size = queue_length(q);
    fprintf(stdout, "(after_deque) size = %d\n", size);
	TEST_ASSERT(ptr == &data1);  
	ret = queue_destroy(q);
	fprintf(stdout, " (destroy)ret = %d\n", ret);
	size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);  */

}
int main(void)
{
	test_create();
	test_queue_simple();

	return 0;
}
