/*
 * Semaphore simple test
 *
 * Test the synchronization of three threads, by having them print messages in
 * a certain order.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <sem.h>
#include <uthread.h>

sem_t sem1;
sem_t sem2;
sem_t sem3;

static void thread3(void *arg)
{
	//printf("entered thread3\n");
	//printf("thread3 called sem_down(sem3)\n");
	sem_down(sem3);		/* Wait for thread1 */
	//printf("---------------------\n");
	printf("thread3\n");
	//printf("---------------------\n");
	//printf("thread3 called sem_up(sem2)\n");
	sem_up(sem2);		/* Unblock thread2 */
}

static void thread2(void *arg)
{
	//printf("entered thread2)\n");
	//printf("thread2 called sem_down(sem2)\n");
	sem_down(sem2);		/* Wait for thread 3 */
	//printf("---------------------\n");
	printf("thread2\n");
	//printf("---------------------\n");
	//printf("thread2 called sem_up(sem1)\n");
	sem_up(sem1);		/* Unblock thread1 */
}

static void thread1(void *arg)
{
	//printf("entered thread1 \n");
	//printf("thread1 called uthread_create(thread2, null)\n");
	uthread_create(thread2, NULL);
	//printf("thread1 called uthread_create(thread3, null)\n");
	uthread_create(thread3, NULL);

	//printf("thread1 called sem_up(sem3)\n");	
	sem_up(sem3);		/* Unblock thread 3 */
	//printf("thread1 called sem_down(sem1)\n");
	sem_down(sem1); 	/* Wait for thread 2 */
	//printf("---------------------\n");
	printf("thread1\n");
	//printf("---------------------\n");
}

int main(void)
{
	/* creating the semaphores */
	/* initial count of semaphore = 0 */
	//printf("creating semaphores \n");
	sem1 = sem_create(0);
	sem2 = sem_create(0);
	sem3 = sem_create(0);
	//printf("uthread_start called \n");
	uthread_start(thread1, NULL);

	//printf("sem_destory called \n");
	sem_destroy(sem1);
	//printf("sem_destory called \n");
	sem_destroy(sem2);
	//printf("sem_destory called \n");
	sem_destroy(sem3);

	//printf("no more semaphores\n");
	return 0;
}