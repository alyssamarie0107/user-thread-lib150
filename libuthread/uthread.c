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

/* global queues */
static queue_t ready_queue; /* threads that are ready to be elected to run will be waiting in this queue */
//static queue_t running_queue; /* threads that are elected to run will be enqueued to this queue */
//static queue_t blocked_queue; /* threads that become blocked will be enqueued to this queue */
static queue_t zombie_queue; /* threads that have a zombie state will be enqueued to this queue */

/*
 * thread control block (TCB)
 * this structure holds information about the single thread 
 * stores the context of the thread (set of registers)
 * information about its stack (stack pointer)
 * information about the state of the thread (running, ready to run, or exited )
 */
struct uthread_tcb {
	uthread_ctx_t *u_context; /* user-level thread context */
	char *stack_ptr; /* pointer to thread stack */
	int thread_state; /* stores the state of the thread */
};

/* static: scope will be restricted to the file it's declared into */
static struct uthread_tcb *new_thread;

/*
struct uthread_tcb *uthread_current(void)
{
	
}

 when running thread yields, its put at the end of the ready queue 
void uthread_yield(void)
{

}

void uthread_exit(void)
{

}
*/

/*
 * uthread_create() makes a thread out of the function passed as parameter 
 * it doesn't call it directly 
 */
int uthread_create(uthread_func_t func, void *arg)
{
	/* allocate memory for the new thread */
	new_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));

	/* allocate memory for stack for the new thread */
	new_thread->stack_ptr = uthread_ctx_alloc_stack();

	/* check if the stack was allocated; return -1 if it wasn't */
	if(new_thread->stack_ptr == NULL) {
		return -1;
	}

	/* set READY state for the newly created thread */
	new_thread->thread_state = THREAD_READY; 

	/* initialize the context of the new thread */
	int ctx_init_status = uthread_ctx_init(new_thread->u_context,new_thread->stack_ptr, func, arg);
	
	/* return -1 if context creation failed */
	if(ctx_init_status == -1) {	
		return -1;
	}

	/* put the new thread in the ready_queue */
	queue_enqueue(ready_queue, &new_thread);
	return 0;
}

/*
 * this function performs three actions 
 * 1. registers the so-far single execution flow of the application as the idle thread that the library can schedule
 * - register an execution flow is starting a "main" thread that enables thread functions and running threads 
 * - main thread is similar to main function in a process 
 * - idle thread = thread that is ready to be executed, once task is complete, goes back to being idle
 * 2. it creates a new thread, the initial thread, as specified by the arguments of the function 
 * 3. executes an infinite loop which:
 * 
 * 
 * for this step, we expect the scheduler to be non-preemptive (do not yield forcefully, rather should yield willingly) 
 * threads must call uthread_yield() in order to ask the library's scheduler to pick and run the next available thread.
 * - when threads call uthread_yield(), library must provide a "round-robin" kind of scheduling as a serve to threads
 * in non-preemptive mode, a non-compliant thread that never yields can keep the processing resource for itself
 */
int uthread_start(uthread_func_t func, void *arg)
{
	/* create the queues by calling the queue_create function from queue.c */
	ready_queue = queue_create();
	zombie_queue = queue_create();
	
	/* 
	 * 2. creates a new thread, initial thread, as specified by argumnents of function 
	 * once the initial thread is created, it can interact with the other functions of the library, create new threads, exit, etc.
	 * OH:
	 * - need to prepare for the fact that if you are going to create multiple threads, then the thread you are in currently needs to somehow exists 
	 * - for all of the other threads that you are going to create dynamically, going to call uthread_create();
	 * 		-> going to allocate a TCB and initialize it in a certain way 
	 * - for the thread that is already running(initial thread), at some point you also need to create a TCB for this guy
	 * - this is what it means when you call uthread_start()
	 * 		-> the thread that is currently running, needs to exist as part of the library as well so we can context switch out of it and get into our scheduling
	 * - we are not creating this initial/current thread by calling uthread_create() bc it should already exist
	 * 		-> need to make some data structures to make it part of the library in general 
	 */

	/* allocate memory for the initial thread */
	struct uthread_tcb *initial_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));

	/* allocate memory for stack for the initial thread */
	initial_thread->stack_ptr = uthread_ctx_alloc_stack();

	/* check if the stack was allocated; return -1 if it wasn't */
	if(new_thread->stack_ptr == NULL) {
		return -1;
	}

	/* 
	 * piazza: professor states that there cannot be a situation where none of the threads are ready
	 * the initial thread should always be ready, since it is the one that's running the idle loop 
	 */
	initial_thread->thread_state = THREAD_READY;

	func(arg);
	//uthread_ctx_switch(initial_thread->u_context, uthread_create(func, arg));

	/* 
	 * 3. executes an infinite loop - idle loop 
	 */
	while (1) {
		/* a.when there are no more idle threads(threads which are ready to run) left in queue, stop the idle loop and returns  */
		if(queue_length(ready_queue) == 0) {
			exit(0);
		}
		/* c. deal with threads that reached completion TCB */
		else if(queue_length(zombie_queue) != 0){
			/* destroy associated TCB */
		}
		/* b. simply yields to next available thread */
		else {
			//thread_yield();
		}
	}
	return 0;
}

/****************************TODO:PHASE 2/3*************************
 void uthread_block(void)
{
}

void uthread_unblock(struct uthread_tcb *uthread)
{
}
 ********************************************************************/