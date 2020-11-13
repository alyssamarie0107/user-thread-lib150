/* TODO Phase 4 */
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

#define TIMER_FREQ 10000 /*10 ms or 100 HZ */
sigset_t signal_set; /* used to represent a signal set */

static struct sigaction new_act, old_act; /* used to change the action taken by a process on receipt of a specific signal */
static struct itimerval new_timer, old_timer; /* struct used to specify when a timer should expire */

/* signal handler that receives alarm signals and forcefully yields the current running thread */
void alarm_handler()
{
	uthread_yield();
}

/* blocking the alarm signals */
void preempt_disable(void)
{
	/* sigprocmask() enables a way to prevent interrupts during critical parts of your code */
	sigprocmask(SIG_BLOCK, &signal_set, NULL);
}

/* enable preemption by unblocking signals of type SIGVTALRM */
void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
}

void preempt_start(void)
{
	/* set up the structure to specify the new action */
	new_act.sa_handler = alarm_handler; /* specifies the action to be associated with signum, assign it pointer to handling function */
	sigemptyset(&new_act.sa_mask); /* sigemptyset() initializes the signal set given by set to empty */
	sigemptyset(&old_act.sa_mask);
	sigemptyset(&signal_set);
	if (sigaddset(&signal_set, SIGVTALRM) == -1)
	{
		perror("Sigaddset failed to add SIGVTALRM to signal_set");
	}
	new_act.sa_flags = 0; /* sa_flags specifies a set of flags which modify the behavior of the signal */

	/* sigaction will save the value of the existing signal hanlder in the struct old_act */
	sigaction(SIGVTALRM, &new_act, &old_act);

	/* configure the timer to expire after 10ms */
	/* current timer value */
	new_timer.it_value.tv_sec = 0;
	new_timer.it_value.tv_usec = TIMER_FREQ; /* 10ms = 10000 microseconds */
	/* ... and every 10ms after that */
	new_timer.it_interval.tv_sec = 0; /* it_interval: current timer interval */
	new_timer.it_interval.tv_usec = TIMER_FREQ;

	setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer); /* setitimer will save the value of the existing timer in the struct old_timer */
}

void preempt_stop(void)
{
	/* restoring previous signal handler by calling sigaction on the old action to restore the previous singal action */
	int act_restored = sigaction(SIGVTALRM, &old_act, NULL);
	/* if and when the user's handler returns normally, the original mask is restored */
	if (act_restored == -1)
	{
		perror("sigaction restoration failure");
	}

	/* 
	 * restoring prev timer configuration 
	 * same principle as restoring the signal handler 
	 */
	int timer_restored = setitimer(ITIMER_VIRTUAL, &old_timer, NULL); /* sets the timer specified as according to old_timer, which is the prev timer saved */
	if (timer_restored == -1)
	{
		perror("setitimer restoration failure");
	}
}
