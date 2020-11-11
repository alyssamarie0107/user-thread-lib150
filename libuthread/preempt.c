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
	
/*signal handler that receives alarm signals*/
//void alarm_handler(int signal) {

//}



void preempt_disable(void)
{
	/* TODO Phase 4 */
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
}

void preempt_start(void)
{
	/*struct used to specify when a timer should expire*/
	//struct itimeinterval timer_info;
	//struct sigaction act;

	/*this is the period between successive timer interrupts; if set to 0, alarm will be sent only once */
	/*number of whole seconds elspased since the starting point */
	//timer_info.it_interval.tv_sec = 0;

	//timer_info.it_interval.tv_usec = HZ;

	/*tis is the period between now and the first timer interrupt; if 0, the alarm is disabled */
	//timer_info.it_value = HZ;

	/*instals a signal handler that receives alarm signals */
	//memset(&act,0, sizeof(act));
	//act.sa_sigaction = &alarm_handler;
	//act.sa_flags = SA_SIGINFO;
	//sigemptyset(&sa.sa_mask);
	//sigaction(SIGSEGV, &alarm_handler, NULL);
	//raise(SIGSEGV);
}

void preempt_stop(void)
{
	/* TODO Phase 4 */
}
