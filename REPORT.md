## Project #2: User-level thread library
**Course:** ECS 150
**Quarter:** Fall 2020
**Authors:** Abigaela Boroica, Alyssa Rodriguez

In this project we implemented a user-level thread library to support
multithreading within the same process. As a prerequisite to the thread library
we implemented a FIFO container (queue) which we used in subsequent steps. 
Moreover, we implemented a semaphore for synchronization, and ultimately, we
implemented preemption to avoid unwanted uncooperative thread behavior.

## Queue API
In the implementation of queue, we used a single linked list. We chose this over
an array to meet the given constraint that all queue operations must have a
time complexity of O(1) for all operations except the delete and iterate
operations, both of which require the traversal of the whole or part of the
list. We created our own comprehensive tester, built on a given skeleton, which
tests the proper behavior of each function of the queue with different data
types.

## Uthread API

## Semaphore API

## Preemption

## Challenges/Limitations
In our initial phase of implementation, it was hard to envision where we would
use the queue; which might have influenced the way we created our queue tester.
Moreover, we found it more challenging to use different scenarios with pointers
(i.e. void *pointer). Another challenge was to understand how thread library
functions are implemented despite understading easier how they are used. 
Also, although having predefined function helps in many ways, it also imposes
limitations in how one should go about implementation; it restricts a bit of
the freedom a developer gets in implementaition choices. However, it is 
understandable that regardless of the academia or the industry, the design
choices might be enforced many times so it makes it a good practice to be able
to follow guidelines.