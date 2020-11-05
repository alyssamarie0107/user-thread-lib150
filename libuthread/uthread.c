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
	char *thread_state;
	
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
	
} 

int uthread_create(uthread_func_t func, void *arg)
{
	/* allocate new stack for the thread */
	struct uthread_tcb *new_stack = uthread_ctx_alloc_stack();

	/* check if the stack was allocated; return -1 if it wasn't */
	if(new_stack == NULL) {
		return -1;
	}

	/* initialize the context of the new thread*/
	int ctx_init_status = uthread_ctx_init(new_stack->u_context,new_stack, func, arg);
	
	/* return -1 if context creation failed */
	if(ctx_init_status == -1) {	
		return -1;
	}

	return 0;

}

/*int uthread_start(uthread_func_t func, void *arg)
{   //maybe we need this queue to push the threads that are ready to run
	//better said to use the queue as FIFO scheduler
	queue_t ready_queue;
	ready_queue = queue_create();
	
	//creates a new initial thread as specified by argumnents of function
	uthread_create(func, arg);
	
	//executes an infinite loop
		//when there are no more threads which are ready to run, stops the idle loop and returns
		//or yileds to next available thread
		//it could deal with threads that reached completion and destroys their associated TCB
	
}

void uthread_block(void)
{
	
}

void uthread_unblock(struct uthread_tcb *uthread)
{
} */