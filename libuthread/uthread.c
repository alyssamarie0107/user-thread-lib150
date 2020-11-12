#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

/* 
 * thread states 
 * utilizing enum to have the proper names of the thread states set to numerical values
 * ex: THREAD_READY = 0, THREAD_RUNNING = 1, etc. 
 * makes program easier to read/maintain
 */
enum thread_state {
    THREAD_READY, /* when a thread is created or when a thread is yielded or preempted, should have this state ready */
    THREAD_RUNNING, /* when a thread is elected to run, set thread state to running */
    THREAD_BLOCKED, /* when a thread does some sort of I/O request, block it, and when it is complete, set back to ready state */
    THREAD_ZOMBIE /* when a thread has a normal/abnormal termination, set thread state to zombie, when the thread is collected, can remove this thread from queue ? */
};

/* global queue */
static queue_t ready_queue;

/* global structure pointers */
static struct uthread_tcb *prev;
static struct uthread_tcb *next;

static struct uthread_tcb *running_thread_ptr; /* ptr points to the currently running thread's TCB */
static struct uthread_tcb *blocked_thread_ptr; /* ptr points to the blocked thread's TCB */


struct uthread_tcb {
    uthread_ctx_t u_context; /* user-level thread context */
    char *stack_ptr; /* pointer to thread stack */
    int thread_state; /* stores the state of the thread */
};

/* get the currently running thread and return ptr of the current thread's TCB */
struct uthread_tcb *uthread_current(void)
{   
    return running_thread_ptr;
}


void uthread_yield(void)
{
    /* check if there are any threads in the queue */
    if (queue_length(ready_queue) != 0) {

        /* assign prev to the current running thread */
        prev = running_thread_ptr;

        /* update prev's thread state to be ready */
        prev->thread_state = THREAD_READY;

        /* insert this thread to the ready queue */
        queue_enqueue(ready_queue, prev);

        /* now get the next available thread in the ready queue */

        queue_dequeue(ready_queue, (void**)&next);

        /* update the next available thread's state */
        next->thread_state = THREAD_RUNNING;

        /* this thread is now elected to be running so assign running_thread_ptr to this thread */
        running_thread_ptr = next;

        /* context switch */
        uthread_ctx_switch(&prev->u_context, &next->u_context);
    }
}

/* 
 * naive verison of uthread_yield () 
 * only difference is that we are not going to enqueue the current running thread to the back of ready queue 
*/
void uthread_exit(void)
{
    //printf("start of uthread_exit()\n");

    struct uthread_tcb *zombie_thread_ptr; /* ptr points to the zombie thread's TCB */

    /* have the zombie thread pointer point to the current running thread */
    zombie_thread_ptr = running_thread_ptr;

    /* update zombie thread state */
    zombie_thread_ptr->thread_state = THREAD_ZOMBIE;

    /* now check if there are any next available threads in queue */
    if (queue_length(ready_queue) != 0) {
        /* if there are, get the next available thread so it can run */
        queue_dequeue(ready_queue, (void**)&next);

        /* update its thread state to be running */
        next->thread_state = THREAD_RUNNING;

        /* assign this thread to running thread ptr */
        running_thread_ptr = next;

        /* assign zombie thread to prev pointer */
        prev = zombie_thread_ptr;

		/* immediately free zombie pointer since it has reached completion */
		free(zombie_thread_ptr);

        /* context switch */
        uthread_ctx_switch(&prev->u_context, &next->u_context);
    }
}

/*
 * uthread_create() makes a thread out of the function passed as parameter 
 * it doesn't call it directly 
 */
int uthread_create(uthread_func_t func, void *arg)
{

	/* allocate memory for a TCB every time a thread is created. The TCB contains whatever you need for each thread */
    struct uthread_tcb *new_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));

	/* check if the stack was allocated; return -1 if it wasn't */
    if(new_thread == NULL) {
        return -1;
    }

    /* allocate memory for stack for the new thread */
    new_thread->stack_ptr = uthread_ctx_alloc_stack();

    /* check if the stack was allocated; return -1 if it wasn't */
    if(new_thread->stack_ptr == NULL) {
        return -1;
    }

    /* initialize the context of the new thread */
    int ctx_init_status = uthread_ctx_init(&new_thread->u_context,new_thread->stack_ptr, func, arg);
    
    /* check if thread ctx was properly initialized */
    if(ctx_init_status == 0) {  
        /* set READY state for the newly created thread */
        new_thread->thread_state = THREAD_READY; 

        /* put the new thread in queue */
        queue_enqueue(ready_queue, new_thread);
        return 0;
    }
    /* if failed to initialize, return -1*/
    else {
        return -1;
    }
}


int uthread_start(uthread_func_t func, void *arg)
{
	/* create the queues by calling the queue_create function from queue.c */
    ready_queue = queue_create();

	/* create idle thread structure */
    struct uthread_tcb idle_thread;

    /* context object (for the "main" thread)*/
    uthread_ctx_t idle_thread_ctx[1];

    /* getcontext() initializes the structure pointed at by ucp to the currently active context */
    getcontext(&idle_thread_ctx[0]);

    idle_thread.stack_ptr = idle_thread_ctx[0].uc_stack.ss_sp;
    idle_thread.u_context = idle_thread_ctx[0];

    /* register idle thread as a running thread */
    idle_thread.thread_state = THREAD_RUNNING;

    /* allocate memory for running thread ptr */
    running_thread_ptr = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
    
    /* running thread ptr should be pointing to the same address as idle_thread_ptr */
    running_thread_ptr = &idle_thread;

    /* create a new thread with func and arg */
    uthread_create(func, arg);
    
    /* executes an infinite loop - idle loop */
    while (1) {

        /* when there are no more idle threads(threads which are ready to run) left in queue, stop the idle loop and returns  */
        if(queue_length(ready_queue) == 0) {
            queue_destroy(ready_queue);
			preempt_stop();
            return 0;
        }
        /* simply yields to next available thread if still threads in ready queue */
        else {

            uthread_yield();
        }
    }
    return 0;
}


 void uthread_block(void)
{

    /* have the blocked thread pointer point to the current running thread */
    blocked_thread_ptr = running_thread_ptr;

    /* update blocked thread state */
    blocked_thread_ptr->thread_state = THREAD_BLOCKED;

    /* now check if there are any next available threads in queue */
    if (queue_length(ready_queue) != 0) {
        /* if there are, get the next available thread so it can run */
        queue_dequeue(ready_queue, (void**)&next);

        /* update its thread state to be running */
        next->thread_state = THREAD_RUNNING;

        /* assign this thread to running thread ptr */
        running_thread_ptr = next;

        /* context switch */
        uthread_ctx_switch(&blocked_thread_ptr->u_context, &next->u_context);
    }
}  

void uthread_unblock(struct uthread_tcb *uthread)
{

    /* puts the unblocked thread back in the ready queue so it can be elected to run again */
    queue_enqueue(ready_queue, uthread);
}