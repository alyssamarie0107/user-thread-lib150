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
queue. The container is stored in a user defined variable of type `struct queue`
which has pointers to the beginning of the queue (`front`), its end (`rear`) and
a pointer that assists in iterating through the queue (`current`). Moreover,
these pointers are of `struct node` type which is composed of a `void *data`
pointer, which holds the content of the queue and accepts any data type due to
the nature of `void`, and a pointer of `struct node` type that points to the
next element in the queue. While our implementation of queue has some of the
standard member functions such as `enqueue`, `dequeue`, constructor `create` and
destructor `destroy`, it does not support functions such as `front`, `back`,
`swap` etc.. It also, has `length` function which servers as both `size` and 
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
 
## Semaphore API
Work in progress...
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