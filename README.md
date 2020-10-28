# user-thread-lib150

# General Info 
- Due Thursday, November 12th, 2020 at 11:59am via Gradescope.

# Objectives 
- Implementing one of the most used containers in system programming (a
  queue/list) as specified by a given API.
- Learning how to test your code, by writing your own testers and maximizing the
  test coverage.
- Understanding how multiple threads can run within the same process: from the
  creation of new threads to their termination, and including how to perform
  context switching during their concurrent execution.
- Implementing one of the most popular synchronization primitives (semaphores)
  as specified by a given API.
- Writing high-quality C code by following established industry standards.

# Introduction 
- **Goal:** understand the idea of threads by implementing a basic user-level
  thread library for Linux.
- Recall, **user-level thread:** threads which the OS does not know about(OS
  only knows and schedules processes, not threads within processes).
- Your library will provide a complete interface for applications to create and
  run independent threads **concurrently**(can start, run, complete in
  overlapping periods, i.e, within the same time lapse of time, we can
  essentially execute a little bit of a certain thread and a lil bit of another
  and so on). 
- Your library must be able to: 
1. Create new threads 
2. Schedule the execution of threads in a **round-robin**(threads run only for a
   (short) time slice at a time) fashion. 
3. Be **preemptive**, that is to provide an interrupted-based scheduler. 

# Constraints 
- Your code must be written in C and be compiled with GCC and only use the std
  functions provided by the GNU C Library (libc).

# Assessment 
- Auto-grading: 60% of grade 
- Manual review: 40% of grade 

# Suggested Work Phases

## Phase 0: Skeleton code 
- Skeleton code is available in **/home/cs150jp/public/p2/**. 
- This code already defines most of the prototypes for the functions you must
  implement, as explained in the following sections.
- The code is organized in 2 parts. 
-  In subdirectory apps, you will find several test applications.
- The tester **queue_tester.c** focuses solely on the queue implementation, and
  can be run after Phase 1 is completed.
- The testers prefixed with **uthread_** make use of the thread library and
  require Phase 2 to be completed.
- The testers prefixed with **sem_** require Phase 3 to be comple
- Subdirectory **libuthread** contains the files composing the thread library
  that you must complete. **The files to complete are marked with a star.**
- **IMPORTANT: You should have no reason to touch any of the headers which are
  not marked with a star (even if you think you do…). **
- Copy the skeleton code to your account.

## Phase 1: queue API
- In this first phase, you must implement a simple FIFO queue.
- The interface to this queue is defined in **libuthread/queue.h** and your code
  should be added into **libuthread/queue.c**.
- You will find all the API documentation within the header file.
- The constraint for this exercise is that all operations (apart from the
  iterate and delete operation) must be O(1). 
- This implies that you must choose the underlying data structure for your queue
  implementation carefully.

### Makefile 
- Complete the file **libuthread/Makefile** in order to generate a *static
  library archive* named **libuthread/libuthread.a**.
- This library archive must be the default target of your Makefile, because your
  Makefile is called from the Makefile in the apps directory without any
  argument.
- Note that at first, only the file **libuthread/queue.c** should be included in
  your library. You will add the other C files as you start implementing them in
  order to expand the API provided by your library.
- Useful resource for this phase:
  http://tldp.org/HOWTO/Program-Library-HOWTO/static-libraries.html

### Testing 
- Add a new test program in the apps directory, called **queue_tester.c**, which
  tests your queue implementation.
- It is important that your tester should be as comprehensive as possible in
  order to ensure that your queue implementation is resistant.
- It will ensure that you don’t encounter bugs when using your queue later on.
- A good approach for testing your queue implementation is *unit testing*.
- The basic idea behind unit testing is to invent all the possible usage
  scenarios that will trigger the different parts of the implementation, and all
  the edge cases, in order to guarantee that the implementation always matches
  the specifications.
- You can find a comprehensive example of a queue tester in
  **/home/cs150jp/public/p2/apps/queue_tester_example.c**, which you can
  complete with more unit tests.

### Hints 
- Most of the functions of this API should look very familiar if you have ever
  coded a FIFO queue (e.g. create, destroy, enqueue, dequeue, etc.).
- However, one function of the API stands out from typical interfaces:
  **queue_iterate()**.
- This function provides a generic way to call a custom function (i.e. a
  function provided by the caller) on each item currently enqueued in the queue.
- Hopefully, you will find that this function can be used in some ways when
  implementing the uthread API. 
- One interesting usage may be, for example, to debug your queue(s) of threads
  by printing them! But other effective usages are possible too…
