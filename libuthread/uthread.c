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
static queue_t thread_queue; /* threads that are ready to be elected to run will be waiting in this queue */
//static queue_t running_queue; /* threads that are elected to run will be enqueued to this queue */
//static queue_t blocked_queue; /* threads that become blocked will be enqueued to this queue */
//static queue_t zombie_queue; /* threads that have a zombie state will be enqueued to this queue */

/*
 * thread control block (TCB)
 * this structure holds information about the single thread 
 * stores the context of the thread (set of registers)
 * information about its stack (stack pointer)
 * information about the state of the thread (running, ready to run, or exited )
 */
struct uthread_tcb {
	uthread_ctx_t u_context; /* user-level thread context */
	char *stack_ptr; /* pointer to thread stack */
	int thread_state; /* stores the state of the thread */
};

/* structure pointers */
static struct uthread_tcb *prev;
static struct uthread_tcb *next;
static struct uthread_tcb *new_thread_ptr; /* ptr points to new thread's TCB */
static struct uthread_tcb *idle_thread_ptr; /* ptr points to main thread's TCB */
static struct uthread_tcb *current_thread_ptr; /* ptr points to the current thread's TCB */

/* get the currently running thread and return ptr of the current thread's TCB */
struct uthread_tcb *uthread_current(void)
{	
	/* get the current thread by dequeuing it from running queue */
	queue_dequeue(thread_queue, (void**)&current_thread_ptr);
	/* however we don't want to actually remove it so put it back in queue */
	queue_dequeue(thread_queue, (void**)&current_thread_ptr);
	return current_thread_ptr;
}

/*
 * when a running thread yields, its put at the end of the ready queue 
 * OH: yield is like "ok im leaving the CPU, who should i schedule next? who is first in the ready queue?"
 * context_switch from the thread that called yield to the next available thread 
 */
void uthread_yield(void)
{
	/* would suspend current thread and put it at the rear of the ready_queue */
	/* called to ask the library's scheduler to pick and run the next available thread */

	/* check if there are any threads in the ready queue */
	if (queue_length(thread_queue) >= 1) {
		/* dequeue the current running thread */
		queue_dequeue(thread_queue, (void**)&prev);

		/* update its thread state */
		prev->thread_state = THREAD_READY;

		/* insert this thread to the back of the ready queue */
		queue_enqueue(thread_queue, prev);

		/* now get the next available thread in the ready queue */
		queue_dequeue(thread_queue, (void**)&next);

		/* update its thread state */
		next->thread_state = THREAD_RUNNING;
		queue_enqueue(thread_queue, next);

		/* context switch */
		uthread_ctx_switch(&prev->u_context, &next->u_context);

	}

	/* dequeue the thread in the running queue */
	printf("dequeue the thread in the running queue\n");
	queue_dequeue(thread_queue, (void**)&prev);

	/* put this thread at the end of the ready queue */
	printf("enqueue the thread we just dequeued to the end of the ready queue \n");
	queue_enqueue(thread_queue, (void**)&prev);

	/* get the next available thread in the ready_queue */
	printf("dequeue the next available thread in read queue \n");
	queue_dequeue(thread_queue, (void**)&next);


}

/* very similar to uthread_yield except that it doesn't enqueue the current thread back into the ready queue*/
void uthread_exit(void)
{
		/* dequeue the current running thread */
		queue_dequeue(thread_queue, (void**)&prev);

		/* update its thread state */
		prev->thread_state = THREAD_ZOMBIE;

		/* enqueue thread to the zombie thread */
		//queue_enqueue(zombie_queue, prev);

		/* now get the next available thread in the ready queue */
		queue_dequeue(thread_queue, (void**)&next);

		/* update its thread state */
		next->thread_state = THREAD_RUNNING;
		queue_enqueue(thread_queue, next);

		/* context switch */
		uthread_ctx_switch(&prev->u_context, &next->u_context);
}

/*
 * uthread_create() makes a thread out of the function passed as parameter 
 * it doesn't call it directly 
 */
int uthread_create(uthread_func_t func, void *arg)
{
	printf("start of uthread_create()\n");
	struct uthread_tcb new_thread;

	/* allocate memory for stack for the new thread */
	new_thread.stack_ptr = uthread_ctx_alloc_stack();

	/* check if the stack was allocated; return -1 if it wasn't */
	if(new_thread.stack_ptr == NULL) {
		return -1;
	}

	/* initialize the context of the new thread */
	int ctx_init_status = uthread_ctx_init(&new_thread.u_context,new_thread.stack_ptr, func, arg);
	
	/* check if thread ctx was properly initialized */
	if(ctx_init_status == 0) {	
		/* set READY state for the newly created thread */
		new_thread.thread_state = THREAD_READY; 

		new_thread_ptr = &new_thread;

		/* put the new thread in the ready_queue */
		queue_enqueue(thread_queue, new_thread_ptr);
		printf("end of uthread_create()\n");
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
	thread_queue = queue_create();
	//running_queue = queue_create();
	//zombie_queue = queue_create();

	/* create idle thread structure */
	struct uthread_tcb idle_thread;

	/* context object (for the "main" thread)*/
	uthread_ctx_t idle_thread_ctx[1];

	/* getcontext() initializes the structure pointed at by ucp to the currently active context */
	getcontext(&idle_thread_ctx[0]);

	idle_thread.stack_ptr = idle_thread_ctx[0].uc_stack.ss_sp;
	idle_thread.u_context = idle_thread_ctx[0];

	/* register initial thread as a runing thread and enqueue it to the running queue */
	idle_thread.thread_state = THREAD_RUNNING;
	idle_thread_ptr = &idle_thread;

	printf("enqueue initial thread to running queue \n");
	/* enqueue idle thread ptr that points to idle thread's TCB 3*/
	queue_enqueue(thread_queue, idle_thread_ptr);
	printf("enqueue complete \n");

	printf("creating new thread from func, arg\n");
	/* create a new thread with func and arg */
	uthread_create(func, arg);
	
	/* executes an infinite loop - idle loop */
	while (1) {
		printf("entered while loop!\n");
		/* when there are no more idle threads(threads which are ready to run) left in queue, stop the idle loop and returns  */
		if(queue_length(thread_queue) == 0) {
			printf("entered empty ready queue\n");
			return 0;
		}
		/* deal with threads that reached completion TCB 
		else if(queue_length(zombie_queue) != 0){

		}
		*/
		/* simply yields to next available thread */
		else {
			printf("entered else uthread_yield statement\n");
			uthread_yield();
		}
	}
	return 0;
}

/****************************TODO:PHASE 2/3*************************
 * very similar to yield, except for yield you would put yourself back of the ready queue bc just yielding and you want to run again
 * with block you wouldnt put yourself back in the ready queue because you are blocked
 * other than that it is very similar to queue 
 * where does a thread go when it is in the block state though?
 * 		- semphore will have a queue of blocked threads that are waiting on it 
 void uthread_block(void)
{
}

void uthread_unblock(struct uthread_tcb *uthread)
{
}
 ********************************************************************/