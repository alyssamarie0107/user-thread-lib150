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
static queue_t zombie_queue;

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

/* structure pointers */
static struct uthread_tcb *prev;
static struct uthread_tcb *next;
static struct uthread_tcb *new_thread_ptr; /* ptr points to new thread's TCB */
static struct uthread_tcb *idle_thread_ptr; /* ptr points to main thread's TCB */
static struct uthread_tcb *running_thread_ptr; /* ptr points to the current thread's TCB */
static struct uthread_tcb *blocked_thread_ptr; /* ptr points to the current thread's TCB */
static struct uthread_tcb *zombie_thread_ptr; /* ptr points to the current thread's TCB */

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
	if (queue_length(ready_queue) > 0) {

		/* assign prev to the current running thread */
		prev = running_thread_ptr;

		/* update prev's thread state to be ready */
		prev->thread_state = THREAD_READY;

		/* insert this thread to the back of the queue */
		//printf("enqueue curr running thread to READY queue \n");
		queue_enqueue(ready_queue, prev);

		/* now get the next available thread in the ready queue */
		//printf("dequeue next available thread from READY queue \n");
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
	zombie_queue = queue_create();

	/* have the zombie thread pointer point to the current running thread */
	zombie_thread_ptr = running_thread_ptr;

	/* update zombie thread state */
	zombie_thread_ptr->thread_state = THREAD_ZOMBIE;

	/* enqueue this thread to the zombie queue and then destroy it using queue_destroy defined in queue.c 
	queue_enqueue(zombie_queue, zombie_thread_ptr);
	queue_destroy(zombie_queue);
	*/

	/* now check if there are any next available threads in queue */
	if (queue_length(ready_queue) > 0) {
		/* if there are, get the next available thread so it can run */
		queue_dequeue(ready_queue, (void**)&next);

		/* update its thread state to be running */
		next->thread_state = THREAD_RUNNING;

		/* assign this thread to running thread ptr */
		running_thread_ptr = next;

		/* context switch */
		uthread_ctx_switch(&zombie_thread_ptr->u_context, &next->u_context);
	}
	//printf("this should not get printed.. right?\n");
}

/*
 * uthread_create() makes a thread out of the function passed as parameter 
 * it doesn't call it directly 
 */
int uthread_create(uthread_func_t func, void *arg)
{
	//printf("start of uthread_create()\n");
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

		/* put the new thread in queue */
		//printf("enqueue new thread to READY queue \n");
		queue_enqueue(ready_queue, new_thread_ptr);
		//printf("end of uthread_create()\n");
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

	/* 
	 * don't have a running queue because running queue will only have 1 thread in it, so doesn't make sense 
	 * running thread should also not be in the ready queue, bc it is running
	 * rather, lets just have a global running thread ptr and have it point to the running thread
	 */

	/* make the idle_thread_ptr point to the idle_thread address */
	idle_thread_ptr = &idle_thread;
	
	/* running thread ptr should be pointing to the same address as idle_thread_ptr */
	running_thread_ptr = idle_thread_ptr;

	//printf("creating new thread from func, arg\n");
	/* create a new thread with func and arg */
	uthread_create(func, arg);
	
	/* executes an infinite loop - idle loop */
	while (1) {
		//printf("entered while loop!\n");
		/* when there are no more idle threads(threads which are ready to run) left in queue, stop the idle loop and returns  */
		if(queue_length(ready_queue) == 0) {
			//printf("entered empty ready queue\n");
			return 0;
		}
		/* deal with threads that reached completion TCB
		else if(queue_length(zombie_queue) != 0){
			printf("destroying zombie thread");
			printf("queue_destroy() called");
			queue_destroy(zombie_queue);
		}
		*/
		/* simply yields to next available thread */
		else {
			//printf("entered else uthread_yield statement\n");
			uthread_yield();
		}
	}
	return 0;
}

/****************************TODO:PHASE 2/3*************************
 * very similar to yield
 * except for yield you would put yourself back of the ready queue bc just yielding and you want to run again
 * with block you wouldnt put yourself back in the ready queue because you are blocked
 * other than that it is very similar to yield
 * where does a thread go when it is in the block state though?
 * 		- semphore will have a queue of blocked threads that are waiting on it 
 *******************************************************************/
 void uthread_block(void)
{
	//printf("entered uthread_block()\n");

	/* have the zombie thread pointer point to the current running thread */
	blocked_thread_ptr = running_thread_ptr;

	/* update zombie thread state */
	blocked_thread_ptr->thread_state = THREAD_BLOCKED;

	/* ? where should i put this thread ? */

	/* now check if there are any next available threads in queue */
	if (queue_length(ready_queue) > 0) {
		/* if there are, get the next available thread so it can run */
		queue_dequeue(ready_queue, (void**)&next);

		/* update its thread state to be running */
		next->thread_state = THREAD_RUNNING;

		/* assign this thread to running thread ptr */
		running_thread_ptr = next;

		/* context switch */
		uthread_ctx_switch(&blocked_thread_ptr->u_context, &next->u_context);
	}
	//printf("this should not get printed.. right?\n");
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	//printf("start of uthread_unblock()\n");
	/* puts the unblocked thread back in the ready queue so it can be elected to run again */
	//printf("enqueue the blocked thread back into the ready queue\n");
	queue_enqueue(ready_queue, uthread);
	//printf("end of uthread_unblock()\n");
}