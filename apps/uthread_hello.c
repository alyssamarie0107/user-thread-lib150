/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

void hello(void *arg)
{
	printf("Hello world!\n");
}

/* this is the "main thread", for it calls uthread_start() */
/* uthread_start() creates a second thread based on whats given as parameter, which is not considered as the "main thread" */
int main(void)
{
	uthread_start(hello, NULL);
	return 0;
}