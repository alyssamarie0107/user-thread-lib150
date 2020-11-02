#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

struct uthread_tcb {
	/* must have TCB info: 
	* - thread ID ??
	* - context of thread (its set of registers & program counter)
	* - pointer to the thread' stack area
	* - info about state of thread(running/ready/blocked/exited) 
	* - pointer  to the process that triggered the creation of this threads ??
	* - pointer to threads created by this thread  ?? */

	
	uthread_ctx_t u_context; /* user-level thread context */
	void *stack_ptr; /* pointer to thread stack */

	
};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2 */
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
}

int uthread_start(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
}

void uthread_block(void)
{
	/* TODO Phase 2/3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 2/3 */
}