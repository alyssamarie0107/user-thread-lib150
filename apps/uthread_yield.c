/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

void thread3(void *arg)
{
	uthread_yield();
	printf("thread3\n");
} 
 
void thread2(void *arg)
{
	uthread_create(thread3, NULL);
	printf("created thread3\n");
	uthread_yield();
	printf("thread2\n");
}

void thread1(void *arg)
{
	uthread_create(thread2, NULL);
	printf("th1 before first yield\n");
	uthread_yield();
	printf("th1 after first yield\n");
	printf("thread1\n");
	uthread_yield();
}

int main(void)
{
	printf("before start\n");
	uthread_start(thread1, NULL);
	printf("after start\n");
	return 0;
}