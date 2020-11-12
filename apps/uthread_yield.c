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
	//printf("thread3 called uthread_create()\n");
	//printf("enter thread3\n");
	//printf("thread3 called uthread_yield()\n");
	uthread_yield();
	//printf("---------------------\n");
	printf("thread3\n");
	//printf("---------------------\n");
}

void thread2(void *arg)
{
	//printf("enter thread2\n");
	//printf("thread2 called uthread_create()\n");
	uthread_create(thread3, NULL);
	//printf("thread2 called uthread_yield()\n");
	uthread_yield();
	//printf("---------------------\n");
	printf("thread2\n");
	//printf("---------------------\n");
}

void thread1(void *arg)
{
	//printf("enter thread1\n");
	//printf("thread1 called uthread_create()\n");
	uthread_create(thread2, NULL);
	//printf("thread1 called uthread_yield()\n");
	uthread_yield();
	//printf("---------------------\n");
	printf("thread1\n");
	//printf("---------------------\n");
	uthread_yield();
}

int main(void)
{
	//printf("main called uthread_start()\n");
	uthread_start(thread1, NULL);
	return 0;
}