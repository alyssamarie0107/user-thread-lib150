#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define which ITIMER_VIRTUAL /* use to set the timer using virtual timer */

sigset_t signal_set;
/*signal handler that receives alarm signals*/
void alarm_signal_handler(int signal) {
	/* when alarm goes off, force the current thread to yield to the next available thread */
	uthread_yield();
}



void preempt_disable(void)
{
	/* block signal of type SIGVTALARM to disable preemption*/
	sigprocmask(SIG_BLOCK, &signal_set, NULL);	
}

void preempt_enable(void)
{
	/* unblock signal of type SIGVTALARM to enable preemption*/
	sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
}

void preempt_start(void)
{
	/*struct used to specify when a timer should expire*/
	struct itimeinterval current_time_value, old_time_value;
	struct sigaction act;

	/*this is the period between successive timer interrupts; if set to 0, alarm will be sent only once */
	/*number of whole seconds elspased since the starting point */


	/*installs a signal handler that receives alarm signals */
	act.sa_sigaction = &alarm_signal_handler;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	sigaction(SIGVTALRM, &alarm_signal_handler, NULL);
	
	current_time_value.it_interval.tv_sec = 0;
    
	/*set the time interval timer to repeat every 10k microseconds */
	current_time_value.it_interval.tv_usec = 10*HZ;

	/*tis is the period between now and the first timer interrupt; if 0, the alarm is disabled */
	
	current_time_value.it_value.tv_sec = 0;
	current_time_value.it_value.tv_sec = 10*HZ;
	/* we need to set the timer so I think we have to call setitimer */ 
	setitimer(which, &current_time_value, &old_time_value);

}
void preempt_stop(void)
{
	/* TODO Phase 4 */
}
