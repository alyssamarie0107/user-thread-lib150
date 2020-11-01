#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#define TEST_ASSERT(assert)             \
do {                                    \
    printf("ASSERT: " #assert " ... "); \
    if (assert) {                       \
        printf("PASS\n");               \
    } else  {                           \
        printf("FAIL\n");               \
        exit(1);                        \
    }                                   \
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
    int data = 3, *ptr, size;
    int data2 = 5;
    queue_t q;

    fprintf(stderr, "*** TEST queue_simple ***\n");

    q = queue_create();

    queue_enqueue(q, &data);
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);

    queue_enqueue(q, &data2);
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);

    //printf("data want matched address: %p\n", &data);
    queue_dequeue(q, (void**)&ptr);
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);
    //printf("ptr: %p\n", ptr);
    TEST_ASSERT(ptr == &data);
}


void test_queue_destroy(void) {
    fprintf(stderr, "*** TEST destroy ***\n");
    int data = 3, size;
    int data2 = 5;
    queue_t q;

    q = queue_create();

    queue_enqueue(q, &data);
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);

    queue_enqueue(q, &data2);
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);

    queue_destroy(q);
    size = queue_length(q);
    fprintf(stdout, "size = %d\n", size);
    TEST_ASSERT(queue_destroy(q)!=-1);
}

queue_t q2;

/* Callback function that increments items */
static void inc_item(void *data)
{
    int *a = (int*)data;
    *a += 1;
}

void test_iterator(void)
{
    fprintf(stderr, "*** TEST iterator ***\n");
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;

    /* Initialize the queue and enqueue items */
    q2 = queue_create();
    for (i = 0; i < 10; i++)
        queue_enqueue(q2, &data[i]);

    /* Increment every item of the queue */
    queue_iterate(q2, inc_item);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q2) == 10);
}


int main(void)
{
    test_create();
    //test_queue_simple();
    //test_queue_destroy();
    test_iterator();
    return 0;
}