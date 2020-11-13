## Project #2: User-level thread library
**Course:** ECS 150 **Quarter:** Fall 2020 **Authors:** Abigaela Boroica, Alyssa
Rodriguez

In this project we implemented a user-level thread library to support
multithreading within the same process. As a prerequisite to the thread library
we implemented a FIFO container (queue) which we used in subsequent steps.
Moreover, we implemented a semaphore for synchronization, and implemented
preemption to avoid unwanted uncooperative thread behavior while sharing
resources.
## Queue API
In the implementation of queue, we used a single linked list. We chose this over
an array to meet the given constraint that all queue operations must have a time
complexity of O(1) for all operations except the delete and iterate operations,
both of which require the traversal of the whole or part of the queue. The
container is stored in a user defined variable of type `struct queue` which has
pointers to the beginning of the queue (`front`), its end (`rear`) and a pointer
that assists in iterating through the queue (`current`). 

Moreover, these pointers are of `struct node` type which is composed of a `void
*data` pointer, which holds the content of the queue and accepts any data type
due to the nature of `void`, and a pointer of `struct node` type that points to
the next element in the queue. While our implementation of queue has some of the
standard member functions such as `enqueue`, `dequeue`, constructor `create` and
destructor `destroy`, it does not support functions such as `front`, `back`,
`swap` etc. It also, has `length` function which servers as both `size` and
`empty` (depending on the usage) that other libraries have as memebr functions.
Additionaly, it supports a feature `iterate` which supports forward iteration
through all elements of the queue while calling a callback function on each
element. Lastly, unlike many ohter libraries, it supports a `delete` method
which supports deletion of an element regardless of its position in the queue.
To test our container, we created our own comprehensive tester, built on a given
skeleton, which tests the proper behavior of each function of the queue by
printing its elements after an enqueue, deque or deletion and by using `assert`
to test the return values of each operations.
## Uthread API
In our uthread implementation, we first declared the possible states a thread
can have using `enum` so that the thread states can be set to numerical values.
We also have a structure that represents a TCB. Inside the `struct` we have the
user-level thread context, pointer to a thread stack, and a thread state defined
in the stucture. We also have a global queue called `ready_queue` and have three
global structure pointers called `prev`, `next`, and `running_thread_ptr`. 

In order to start the uthread_library, `uthread_start();` must be called. In
this function we create the ready queue, and register the single execution flow
of the application as the idle thread by defining a context object for this
thread and initializing it. After this, we assign the `running_thread_ptr` to
the idle thread we just initialized. Next, we call `uthread_create();` to create
a new thread out of the function and argument passed as parameter. 

In `uthread_create();` we allocate a pointer of type `struct uthread_tcb`. We
then allocate memory for this new thread's stack by calling
`uthread_ctx_alloc_stack();` and assign this new thread's stack pointer to it.
Additionally, we initilize the context of the new thread by utilizing
`uthread_ctx_init();`. Once the thread has been sucessfully created and
initialized, we enqueued it in our ready queue by using `queue_enqueue();`.
Returning back to `uthread_start();` we also define an infinite loop that checks
if the ready queue has anymore threads in it. If it doesn't, it returns 0,
otherwise, it calls `uthread_yield();`

`uthread_yield();`, `uthread_exit();`, and `uthread_block();` are very similar,
thus our implementations to all these look fairly identical. In
`uthread_yield();` we assign the structure pointer `prev` to
`running_thread_ptr` and update its state to ready. We then enqueue `prev` in
the ready queue and dequeue the next available thread using `queue_dequeue();`.
This thread is now assigned to `running_thread_ptr` and we do a context switch.
```
uthread_ctx_switch(&prev->u_context, &next->u_context);
```
The only difference with `uthread_exit();` is that we don't enqueue the
currently running thread to the ready queque anymore. Instead, we assign it to a
structure pointer called `zombie_thread_ptr`. This zombie thread pointer is
immediately freed since it essentially represents a thread reaching completion.
In regard to `uthread_block();`, the only difference is that the currently
running thread is not enqueued to the ready queue right away, in comparison to
immediately(`uthread_yield()`) or never again(`uthread_exit();`). The only time
this blocked thread is able to get back on the ready queue is if it gets
unblocked by `uthread_unblock();`. `uthread_unblock()` enqueues the blocked
thread back on the ready queue. 
```
queue_enqueue(ready_queue, uthread);
```
`uthread` is the parameter and its value comes from the semaphore API,
specifically `sem_up();`. This API is separated from the semaphore API, but the
semaphore API is able to get the currently running thread from the uthread.c
file by calling `uthread_current();`. In this function, we simply return
`running_thread_ptr`. 
## Semaphore API
In our implementation of semaphore we used a counted semaphore of user defined
type `struct semaphore` (or `sem_t`). This structure contains an internal
counter that keeps track of the available resources and gets updated when a
thread is taking over or releases a shared resource. It also has a waiting list
in which threads, that try to take a resource when the internal counter is 0 or
less, are blocked from execution and are placed into it. Once a new resource
becomes available,the first thread in the waiting list will get elected to
resume its execution. That is why our waiting list is of `queue_t` type since we
need it to function in a FIFO manner. 

In terms of methods, our semaphore has a constructor (`sem_create`) and a
destructor (`sem_destroy`) in which our semaphore object gets initialized and
deallocated respectively. `sem_down` is the method which blocks the execution of
a thread that tries to get a resource that does not exist (the internal counter
is zero) and puts it on its own waitlist. If there are resoruces still, it
decrements its internal counter. `sem_up` increments the internal counter when a
resoruce becomes available and removes from the waitlist the first thread to
resume its execution. To accomplish its purposes fully, our semaphore needed a
way to communicate with out thread API; it does that by calling `uthread_block`
in `sem_down` when it blocks thread execution or by using a `struct uthread_tcb`
pointer that stores the first thread in the waiting list and is fed as argument
to `uthread_block` to resume its execution.
## Preemption API
The purpose of the preemption API is to forcefully suspend a thread even during
long CPU bursts by the use of a timer interrupt. The timer we implement in our
API is configured in the function, `preempt_start();` The struct `itimerval` was
what we used to make our timer fire an alarm a hundred times per second (i.e 100
Hz or 10ms). The `setitimer();` function, which is declared in the header file
`sys/time.h` is the primary means for setting our alarm. 
```
setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer); 
```
Since we wanted to count the processor time used by a thread, we made a virtual
timer, in which we specify by using `ITIMER_VIRTUAL` as the first argument of
`setitimer();`. The last two arguments `setitimer();` takes are essentially a
new timer, which is the timer we configured using `itimerval`, and a old timer.
The old timer argument can in fact be declared as `NULL`, however, we avoided
doing this for the purpose of restoring the timer. Additionally, in
`preempt_start();`, we installed a alarm handler to take care of the signal that
the timer sends. The way we set up the structure is to specify the action of
handling the signal as well as how we make use of the `sigaction();` function is
shown below. 
```
new_act.sa_handler = alarm_handler;
sigemptyset(&new_act.sa_mask);
sigemptyset(&old_act.sa_mask);
new_act.sa_flags = 0;
sigaction(SIGVTALRM, &new_act, &old_act);
```
The first line in the above code block indicates the action to be associated
with the signal. `new_act.sa_handler` is assigned a function pointer and the
only thing in our `alarm_handler();` function is `uthread_yield();`; which again
puts the currently running thread to the back of the ready queue so that the
next available thread in the queue gets elected to run on the processor.
`sigemptyset()` initializes the signal, while `sa_flags`specifies a set of flags
which modify the behavior of the signal. The first argument in `sigaction();` is
`SIGVTALRM`, which indicates expiration of a timer that measures CPU time used
by the thread. In a similar manner to `setitimer();`, `sigaction();` also takes
in an new arg that is used to set up a new action for the signal as well as an
old arg that is used to return info about the action previously associated with
the signal, if it is not declared. Likewise, the third argument can be `NULL`,
but we use old_act to achieve restoring the signal handler. 

Restoring the previous action and timer configuration is mostly done in our
function, `preempt_stop();`. We define `new_act`, `old_act`, `new_timer`,
`old_timer` as global structures because after `sigaction();` and `setitimer();`
have finished saving the previous signal handler and timer configuration in
`old_act` and `old_timer`, `preempt_stop();` needs access to them as well. In
`preempt_stop();`, we simply call `sigaction();` and `setitimer();` again, but
this time on the old action and old timer to restore these values. 
```
sigaction(SIGVTALRM, &old_act, NULL);
setitimer(ITIMER_VIRTUAL, &old_timer, NULL);
```
The last two functions in our preemption API are `preempt_disable();` and
`preempt_enable();`. `preempt_disable();` blocks signals of type `SIGVTALRM` by
utilizing the function `sigprocmask();`, with the first argument set to
`SIG_BLOCK`, which enables a way to prevent interrupts during critical sections
of code. Similarly, `preempt_enable();` uses `sigprocmask();` as well, but with
the first argument set to `SIG_UNBLOCK`, which unblock signals, in this case of
type `SIGVTALRM`. It should be noted we use `preempt_enable();` and
`preempt_disable();` in the critical sections of our uthread.c and sem.c files
and call `preempt_start();` in our `uthread_create();` function and call
`preempt_stop();` before `uthread_start();` returns. 
## test_preempt.c
Our tester, `test_preempt.c`, shows that our implementation for preemption works
because once `thread1` is created by calling `uthread_start();` in `main`, we
enter the `thread1();` function which contains creates a new thread, `thread2`,
and then enters an infinite loop. It does not call yield nor calls sem_down() on
a semaphore with a internal count of 0, thus `thread1` would essentially run
forever. However, this is not the case, because since we did implement
preemption, `thread1` gets forcefully interrupted and `thread2` gets elected to
run. `thread2` calls `exit(0);` and terminates the process. 
## Challenges/Limitations
In our initial phase of implementation, it was hard to envision where we would
use the queue; which might have influenced the way we created our queue tester.
Moreover, we found it more challenging to use different scenarios with pointers
(i.e. void *pointer) since we are still navigating our way with programming in
C. Another challenge we faced was understanding the concepts and functionalities
of threads and integrating our developed knowledge to implement the thread
library functions. Oftentimes, we were stuck and didn't even know where to start
once. However, re-reading the lecture slides, asking questions on Piazza,
meeting on Zoom with each other almost every day, and attending office hours
helped us tremendously with ability to finish strong.
## Cited Resources
- Lecture slides
- The GNU C Library
- https://www.youtube.com/watch?v=XuCbpw6Bj1U
- https://www.informit.com/articles/article.aspx?p=23618&seqNum=14
- https://stackoverflow.com/questions/11344547/how-do-i-compile-a-static-library
- https://www.ibm.com/support/knowledgecenter/SSLTBW_2.4.0/com.ibm.zos.v2r4.bpxbd00/rstimr.htm
