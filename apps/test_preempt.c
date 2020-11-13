#include <stdio.h>
#include <stdlib.h>

#include "private.h"
#include "uthread.h"

/*
 * testing preemption:
 * since thread1 wants to run a while loop forever and hog all the resources, 
 * it will be interrupted by timer to let thread2 run 
 * when thread2 runs, it should terminate the program, which sucessfully shows 
 * that our preemption API works!
 */

static void thread2(void *arg)
{
    exit(0);
}

static void thread1(void *arg)
{
    preempt_start();               /* sets up timer and alarm handler */
    uthread_create(thread2, NULL); /* create a new thread, so thread2 will be in the ready queue */
    /* not going to call uthread_yield or call sem_down() on a 0 sempahore */
    while (1)
    {
        printf("i want the CPU forever!\n");
    }
}

int main(void)
{
    /* 
     * when creating thread1 by using uthread_create(), it calls uthread_ctx_init(); 
     * uthread_ctx_init() calls uthread_ctx_bootstrap().
     * in uthread_ctx_bootstrap(), this is where preempt_enable() is used 
     * preempt_enable() allows a thread to be forcefully interrupted 
     */
    uthread_start(thread1, NULL);
    return 0;
}