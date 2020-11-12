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
 * alarm is fired every 1/100 second (convert to ms) -> 10ms
 */

sigset_t signal_set; /* used to represent a signal set */

static struct sigaction new_act, old_act; /* used to change the action taken by a process on receipt of a specific signal */
static struct itimerval new_timer, old_timer; /* struct used to specify when a timer should expire */

/* signal handler that receives alarm signals and forcefully yields the current running thread */
void alarm_handler() {
	//printf("in alarm handler going to call uthread_yield()\n");
	uthread_yield();
}

/* 
 * disable preemption by blocking signals of type SIGVTALRM 
 * means only temporarily blocking the alarm signals 
 */
void preempt_disable(void)
{
	/* 
	 * sigprocmask() enables a way to prevent interrupts during critical parts of your code 
	 * if signal arrive in that part of the program, they are delivered later, after you unblock them 
	 */
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
	sigemptyset (&signal_set);
	if(sigaddset(&signal_set, SIGVTALRM) != 0) {
   		printf("sigaddset did not successfully add SIGVTALRM\n");
	}
	/* sa_mask specifies a mask of signals which should be blocked during execution of the signal handler */
	new_act.sa_flags = 0; /* sa_flags specifies a set of glags which modify the behavior of the signal */

	sigaction(SIGVTALRM, &new_act, &old_act);

	/* configure the timer to expire after 10ms */
	/* current timer value */
	new_timer.it_value.tv_sec = 0;
	new_timer.it_value.tv_usec = 10000; /* 10ms = 10000 microseconds */
	/* ... and every 10ms after that */
	new_timer.it_interval.tv_sec = 0; /* it_interval: current timer interval */
	new_timer.it_interval.tv_usec = 10000;
	
	setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer);
}


void preempt_stop(void)
{
	/* restoring previous signal handler by calling sigaction on the old action to restore the previous singal action */
	int act_restored = sigaction(SIGVTALRM, &old_act, NULL);
	/* if and when the user's handler returns normally, the original mask is restored */
	if (act_restored == 0){
		//printf("singal handler restored successfully\n");
	}
	else {
		perror("sigaction() failed restoring signal handler\n");
	}

	/* 
	 * restoring prev timer configuration 
	 * according to piazza it is the same principle as restoring the signal handler 
	 */
	int timer_restored = setitimer(ITIMER_VIRTUAL, &old_timer, NULL);
	if (timer_restored == 0){
		//printf("timer restored successfully\n");
	}
	else {
		perror("setitimer() failed restoring signal handler\n");
	}
}