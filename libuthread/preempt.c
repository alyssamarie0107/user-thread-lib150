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

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 * alarm is fired every 1/100 second (convert to ms) -> 10ms
 */

sigset_t signal_set; /* used to represent a signal set */

static int act_restored; /* 0 = failed to restore, 1 = restored successfully */
static int timer_restored; /* 0 = failed to restore, 1 = restored successfully */

static struct sigaction new_act, old_act; /* used to change the action taken by a process on receipt of a specific signal */
static struct itimerval new_timer, old_timer; /* struct used to specify when a timer should expire */

/* signal handler that receives alarm signals and forcefully yields the current running thread */
void alarm_handler(int signum) {
	uthread_yield();
}

/* 
 * disable preemption by blocking signals of type SIGVTALRM 
 * means only temporarily blokcing the alarm signals 
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
	//act.sa_sigaction = alarm_handler;
	sigemptyset(&new_act.sa_mask); /* sigemptyset() initializes the signal set given by set to empty */
	sigemptyset(&old_act.sa_mask);
	/* sa_mask specifies a mask of signals which should be blocked during execution of the signal handler */
	new_act.sa_flags = 0; /* sa_flags specifies a set of glags which modify the behavior of the signal */

	/* 
	 * sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
	 * new arg is used to set up a new action for the signal signum 
	 * old arg is used to return info about the action previously associated with this signal
	 * mandatory to use sigaction() over signal() 
	 * same basic effect as signal: to specify how a signal show be handled by the process
	 * however, sigaction offers more control, at the expense of more complexity 
	 * signum = specifies the signal and can be any valid signal except SIGKILL and SIGSTOP
	 * return value from sigaction is zero if it succeeds, and -1 on failure
	 */

	/* if and when the user's handler returns normally, the original mask is restored */
	if (sigaction(SIGVTALRM, &new_act, &old_act) == 0){
		act_restored = 1;
	}
	else {
		perror("sigaction");
	}

	/* configure the timer to expire after 10ms */
	/* it_interval: current timer interval */
	/* current timer value */
	new_timer.it_value.tv_sec = 0;
	new_timer.it_value.tv_usec = 10000; /* 10ms = 10000 microseconds */
	/* ... and every 10ms after that */
	new_timer.it_interval.tv_sec = 0;
	new_timer.it_interval.tv_usec = 10000;
	
	/* 
	 * int setitimer(int which, struct itimerval *new, struct itimerval *old)
	 * if old_timer is not a null pointer, setitimer returns information about any previous 
	 * unexpired timer of the same kind in the structure it points to.
	 * setitimer() sets the value of an interval timer 
	 * an interval timer is a timer which sends a signal after each repition (interval) of time 
	 * which argument indicates what kind of time is being controlled 
	 * ITIMER_VIRTUAL: marking process virtual time 
	 * process virtual time is the amount of time spent while executing in the process, can be thought of as a CPU timer 
	 * SIGVTALRM signal is generate after each interval of time 
	 * count down whenever this process is executing
	 * return value is 0 on success and -1 on failure
	 */
	if (setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer) == 0){
		timer_restored = 1;
	}
	else {
		perror("sigaction");
	}
}

/* restore previous timer configuration and previous action associate to virtual alarm signals */
void preempt_stop(void)
{
	/* if and when the user's handler returns normally, the original mask is restored */
	if (act_restored == 1){
		/* calling sigaction on the old action to restore the previous singal action */
		/*
	    * Piazza: "what I understand is that sigaction() will somehow give back the action 
	 	* previously associated with the signal and we can call sigaction() 
	 	* on the old action to restore the previous signal action. I'm not sure if it's correct. 
	 	* If so, does it mean that the signal handler and the alarm also be removed?"
	 	* Professor's response: "Your understanding is correct. Try it!
	 	* sigaction will only affect the signal handler. the alarm must be removed via another function 
	 	* (but the principle is the same)"
	 	*/
		sigaction(SIGVTALRM, &old_act, NULL);
	}
	else {
		perror("sigaction() failed restoring signal handler");
	}
	
	if (timer_restored == 1) {

	}
	else {
		perror("sigaction() failed restoring timer");
	}
}