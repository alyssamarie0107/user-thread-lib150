#include <stdio.h>
#include <stdlib.h>

#include "private.h"
#include "uthread.h"

static counter = 0;
static void thread(void *arg) {
    preempt_disable();
    counter++;
    preempt_enable();
    printf("thread3: counter = %d\n",counter);
}
static void thread2(void *arg) {
    uthread_create(thread3, NULL);
    preempt_disable();
    counter++;
    preempt_enable();
    printf("thread2: counter = %d\n",counter);
}
static void thread1(void *arg) {
    preempt_start();
    uthread_create(thread2, NULL);
    preempt_disable();
    counter++;
    preempt_enable();
    printf("thread1: counter = %d\n",counter);
}

int main(void) {
    
    uthread_start(thread1, NULL);
}