/* TODO Phase 2 */
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

/*
 * thread control block (TCB)
 * this structure holds information about the single thread 
 * stores the context of the thread (set of registers)
 * information about its stack (stack pointer)
 * information about the state of the thread (running, ready to run, or exited)
 */
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

/*
 * when a running thread yields, its put at the end of the ready queue 
 * OH: yield is like "ok im leaving the CPU, who should i schedule next? who is first in the ready queue?"
 * context_switch from the thread that called yield to the next available thread 
 */
void uthread_yield(void)
{
    /* check if there are any threads in the queue */
    if (queue_length(ready_queue) != 0) {

		/* CRITICAL SECTION : modifying global queue and struct ptrs -> prev, next, running_thread_ptr */
		preempt_disable(); /* disable forced interrupts */

		/* assign prev to the current running thread */
        prev = running_thread_ptr;

        /* update prev's thread state to be ready */
        prev->thread_state = THREAD_READY;

        /* insert this thread to the ready queue */
        /* printf("enqueue curr running thread to READY queue \n"); */
        queue_enqueue(ready_queue, prev);

        /* now get the next available thread in the ready queue */
        /* printf("dequeue next available thread from READY queue \n"); */
        queue_dequeue(ready_queue, (void**)&next);

        /* update the next available thread's state */
        next->thread_state = THREAD_RUNNING;

        /* this thread is now elected to be running so assign running_thread_ptr to this thread */
        running_thread_ptr = next;

		/* END OF CRITICAL SECTION */
		preempt_enable(); /* done modifying global variables, thus enable preemption */

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
    /* printf("start of uthread_exit()\n"); */

    struct uthread_tcb *zombie_thread_ptr; /* ptr points to the zombie thread's TCB */

    /* have the zombie thread pointer point to the current running thread */
    zombie_thread_ptr = running_thread_ptr;

    /* update zombie thread state */
    zombie_thread_ptr->thread_state = THREAD_ZOMBIE;

    /* now check if there are any next available threads in queue */
    if (queue_length(ready_queue) != 0) {
		/* CRITICAL SECTION : modifying global queue and struct ptrs -> prev, next, running_thread_ptr */
		preempt_disable(); /* disable forced interrupts */

        /* if there are threads in ready queue, get the next available thread so it can run */
        queue_dequeue(ready_queue, (void**)&next);

        /* update its thread state to be running */
        next->thread_state = THREAD_RUNNING;

        /* assign this thread to running thread ptr */
        running_thread_ptr = next;

        /* assign zombie thread to prev pointer */
        prev = zombie_thread_ptr;

		/* END OF CRITICAL SECTION */
		preempt_enable(); /* done modifying global variables, thus enable preemption */

		/* immediately free zombie pointer since it has reached completion */
		free(zombie_thread_ptr);

        /* context switch */
        /* printf("context switch\n"); */
        uthread_ctx_switch(&prev->u_context, &next->u_context);
    }
}

/*
 * uthread_create() makes a thread out of the function passed as parameter 
 * it doesn't call it directly 
 */
int uthread_create(uthread_func_t func, void *arg)
{
    /* printf("start of uthread_create()\n"); */

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
    
	/* preempt_start() should be called when the uthread library is initializing and sets up preemption */
	preempt_start();

    /* check if thread ctx was properly initialized */
    if(ctx_init_status == 0) {  
        /* set READY state for the newly created thread */
        new_thread->thread_state = THREAD_READY; 

        /* printf("enqueue new thread to READY queue \n"); */

		/* CRITICAL SECTION : modifying global queue  */
		preempt_disable(); /* disable forced interrupts */

        queue_enqueue(ready_queue, new_thread); /* put the new thread in queue */
		
		/* END OF CRITICAL SECTION */
		preempt_enable(); /* done modifying global variables, thus enable preemption */
        /* printf("end of uthread_create()\n"); */
        return 0;
    }
    /* if failed to initialize, return -1*/
    else {
        return -1;
    }
}

/*
 * this function performs three actions 
 * 1. registers the so-far single execution flow of the application as the idle thread that the library can schedule
 * 2. it creates a new thread, the initial thread, as specified by the arguments of the function 
 * 3. executes an infinite loop
 *   
 * for this step, we expect the scheduler to be non-preemptive (do not yield forcefully, rather should yield willingly) 
 * threads must call uthread_yield() in order to ask the library's scheduler to pick and run the next available thread.
 * - when threads call uthread_yield(), library must provide a "round-robin" kind of scheduling as a service to threads
 * in non-preemptive mode, a non-compliant thread that never yields can keep the processing resource for itself
 * 
 * the main function that calls uthread_start() in the first place, becomes the idle thread in the sense that 
 * inside of uthread_start() and after creating the first thread, it goes into a while loop that always yields until there are no more threads in the system.
 */
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

	/* CRITICAL SECTION : modifying global struct ptr -> running_thread_ptr */
	preempt_disable(); /* disable forced interrupts */

    /* allocate memory for running thread ptr */
    running_thread_ptr = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
    
    /* running thread ptr should be pointing to the same address as idle_thread_ptr */
    running_thread_ptr = &idle_thread;

	/* END OF CRITICAL SECTION */
	preempt_enable(); /* done modifying global variables, thus enable preemption */

    /* printf("creating new thread from func, arg\n"); */
    /* create a new thread with func and arg */
    uthread_create(func, arg);
    
    /* executes an infinite loop - idle loop */
    while (1) {
        /* printf("entered while loop!\n"); */
        /* when there are no more idle threads(threads which are ready to run) left in queue, stop the idle loop and returns  */
        if(queue_length(ready_queue) == 0) {
			preempt_stop();
            /* printf("NO MORE THREADS IN READY QUEUE, JOB IS DONE!\n"); */
            queue_destroy(ready_queue);
            return 0;
        }
        /* simply yields to next available thread if still threads in ready queue */
        else {
           /* printf("entered else uthread_yield statement\n"); */
            uthread_yield();
        }
    }
    return 0;
}

/*
 * TODO:PHASE 2/3
 * blocking means that the thread stops being in the ready queue
 * thread won't be scheduled to run anymore until another thread unblocks it
 * threads get blocked when they call sem_down() on a semaphore which count is 0 
 * a thread that is blocked on a semaphore should eventually be unblocked 
 * by another thread calling sem_up on the same semaphore. 
 */
 void uthread_block(void)
{
    /* printf("entered uthread_block()\n"); */

	static struct uthread_tcb *blocked_thread_ptr; /* ptr points to the blocked thread's TCB */

    /* have the blocked thread pointer point to the current running thread */
    blocked_thread_ptr = running_thread_ptr;

    /* update blocked thread state */
    blocked_thread_ptr->thread_state = THREAD_BLOCKED;

    /* now check if there are any next available threads in queue */
    if (queue_length(ready_queue) != 0) {
		/* CRITICAL SECTION : modifying global queue and struct ptrs -> next, running_thread_ptr */
		preempt_disable(); /* disable forced interrupts */

        /* if there are, get the next available thread so it can run */
        queue_dequeue(ready_queue, (void**)&next);

        /* update its thread state to be running */
        next->thread_state = THREAD_RUNNING;

        /* assign this thread to running thread ptr */
        running_thread_ptr = next;
		
		/* END OF CRITICAL SECTION */
		preempt_enable(); /* done modifying global variables, thus enable preemption */

        /* context switch */
        /* printf("context switch\n"); */
        uthread_ctx_switch(&blocked_thread_ptr->u_context, &next->u_context);
    }
}  

void uthread_unblock(struct uthread_tcb *uthread)
{
    /* printf("start of uthread_unblock()\n"); */
    
    /* printf("enqueue the blocked thread back into the ready queue\n"); */

	/* CRITICAL SECTION : modifying global queue */
	preempt_disable(); /* disable forced interrupts */

	/* puts the unblocked thread back in the ready queue so it can be elected to run again */
    queue_enqueue(ready_queue, uthread);

	/* END OF CRITICAL SECTION */
	preempt_enable(); /* done modifying global variables, thus enable preemption */
    
	/* printf("end of uthread_unblock()\n"); */
}