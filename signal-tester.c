#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

struct sigaction new_act, old_act; /* used to change the action taken by a process on receipt of a specific signal */
struct itimerval new_timer, old_timer; /* struct used to specify when a timer should expire */

void alarm_handler(int signum)
{
	printf("\nBeep, beep, beep!\n");
	static int count = 0;
 	printf ("timer expired %d times\n", ++count);
}

int main(void)
{
	/* set up handler for the alarm */
	new_act.sa_handler = alarm_handler; /* specifies the action to be associated with signum, assign it pointer to handling function */
	//act.sa_sigaction = alarm_handler;
	sigemptyset(&new_act.sa_mask); /* sigemptyset() initializes the signal set given by set to empty */
	sigemptyset(&old_act.sa_mask);
	/* sa_mask specifies a mask of signals which should be blocked during execution of the signal handler */
	new_act.sa_flags = 0; /* sa_flags specifies a set of glags which modify the behavior of the signal */
	// act.sa_flags = SA_SIGINFO;

	/* 
	 * mandatory to use sigaction() over signal() 
	 * same basic effect as signal: to specify how a signal show be handled by the process
	 * however, sigaction offers more control, at the expense of more complexity 
	 * sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
	 * signum = specifies the signal and can be any valid signal except SIGKILL and SIGSTOP
	 * SIGVTALRM: 
	 */
	sigaction(SIGVTALRM, &new_act, &old_act);

	/* configure the timer to expire after 10ms */
	/* it_interval: current timer interval */
	/* current timer value */
	new_timer.it_value.tv_sec = 0;
	new_timer.it_value.tv_usec = 10000; /* 10ms = 10000 microseconds */
	/* ... and every 10ms after that */
	new_timer.it_interval.tv_sec = 0;
	new_timer.it_interval.tv_usec = 10000;

	/* 
	 * setitimer() sets the value of an interval timer 
	 * an interval timer is a timer which sends a signal after each repition (interval) of time 
	 * int setitimer(int which, struct itimerval *value, struct itimerval *ovalue);
	 * which argument indicates what kind of time is being controlled 
	 * ITIMER_VIRTUAL: marking process virtual time 
	 * process virtual time is the amount of time spent while executing in the process, can be thought of as a CPU timer 
	 * SIGVTALRM signal is generate after each interval of time 
	 * count down whenever this process is executing
	 */
	setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer);

	/* do busy work */
	while(1);
}
