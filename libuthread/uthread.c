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

enum thread_state {
	THREAD_READY, /* when a thread is created or when a thread is yielded or preempted, should have this state ready */
	THREAD_RUNNING, /* when a thread is elected to run, set thread state to running */
	THREAD_BLOCKED, /* when a thread does some sort of I/O request, block it, and when it is complete, set back to ready state */
	THREAD_ZOMBIE /* when a thread has a normal/abnormal termination, set thread state to zombie, when the thread is collected, can remove this thread from queue ? */
};

/* make ready_queue global */

static queue_t ready_queue; 
//static queue_t current_threads;
static queue_t zombie_queue;
static struct uthread_tcb *new_thread;

struct uthread_tcb {
	/* must have TCB info: 
	* - thread ID ??
	* - context of thread (its set of registers & program counter)
	* - pointer to the thread' stack area
	* - info about state of thread(running/ready/blocked/exited) 
	* - pointer  to the process that triggered the creation of this threads ??
	* - pointer to threads created by this thread  ?? */
	
	uthread_ctx_t *u_context; /* user-level thread context */
	char *stack_ptr; /* pointer to thread stack */
	int thread_state; /* stores the state of the thread */
	
};

/*struct uthread_tcb *uthread_current(void)
{	
	
}

void uthread_yield(void)
{
	//would suspend current thread and put it at the rear of the ready_queue
	//called to ask the library's scheduler to pick and run the next available thread
	
} */

void uthread_exit(void)
{
	//change the state of the thread to zombie
	//destroy the associated TCB
	//using assert(0) to exit the thread ???
} 

int uthread_create(uthread_func_t func, void *arg)
{
	/* allocate memory for the new thread thread */
	new_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));

	/* allocate memoory for stack for the new thread */
	new_thread->stack_ptr = uthread_ctx_alloc_stack();

	/* set READY state for the newly created thread */
	new_thread->thread_state = THREAD_READY; 

	/* check if the stack was allocated; return -1 if it wasn't */
	if(new_thread->stack_ptr == NULL) {
		return -1;
	}

	/* initialize the context of the new thread*/
	int ctx_init_status = uthread_ctx_init(new_thread->u_context,new_thread->stack_ptr, func, arg);
	
	/* return -1 if context creation failed */
	if(ctx_init_status == -1) {	
		return -1;
	}

	/* put the new thread in the ready_queue */
	queue_enqueue(ready_queue, &new_thread);
	return 0;

}

int uthread_start(uthread_func_t func, void *arg)
{   //maybe we need this queue to push the threads that are ready to run
	//better said to use the queue as FIFO scheduler
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
	//struct uthread_tcb *initial_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	ucontext_t initial_thread[1];
	getcontext(&initial_thread[0]);

	/* allocate memory for stack for the initial thread */
	//initial_thread->stack_ptr = uthread_ctx_alloc_stack();

	/* check if the stack was allocated; return -1 if it wasn't */
	/*if(new_thread->stack_ptr == NULL) {
		return -1;
	} */

	/* 
	 * piazza: professor states that there cannot be a situation where none of the threads are ready
	 * the initial thread should always be ready, since it is the one that's running the idle loop 
	 */
	//initial_thread->thread_state = THREAD_READY;

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

/*void uthread_block(void)
{
	//do you call block within yield()?
	//block a thread when it deals with I/O so another to execute
}

void uthread_unblock(struct uthread_tcb *uthread)
{
} */