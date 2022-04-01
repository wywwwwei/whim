// Code pratices learning from libuv - queue 
// https://github.com/libuv/libuv

#include <stddef.h>

// typedef QUEUE as an array which contains 2 pointers to void
typedef void *QUEUE[2];

// &((*(q))[0]) where q's type go from
// QUEUE* --dereference-> QUEUE --index-> QUEUE[0] --reference-> void**
#define QUEUE_NEXT(q) (*(QUEUE **)&((*(q))[0]))
#define QUEUE_PREV(q) (*(QUEUE **)&((*(q))[1]))
#define QUEUE_PREV_NEXT(q) (QUEUE_NEXT(QUEUE_PREV(q)))
#define QUEUE_NEXT_PREV(q) (QUEUE_PREV(QUEUE_NEXT(q)))

// the offset in bytes of a structure member
// from the beginning of the structure
#ifndef offsetof
#define offsetof ((int)&(((type *)(0))->field))
#endif

#define QUEUE_DATA(ptr, type, field) \ 
 (type *)((char *)(ptr)-offsetof(type, field))

#define QUEUE_FOREACH(q, h) \
    for ((q) = QUEUE_NEXT(h); (q) != (h); (q) = QUEUE_NEXT(q))

// q->next = q means empty
#define QUEUE_EMPTY(q) \
    (const QUEUE *)(q) == (const QUEUE *)(QUEUE_NEXT(q))

// head node of the cyclic queue
#define QUEUE_HEAD(q) (QUEUE_NEXT(q))

#define QUEUE_INIT(q)        \
    do                       \
    {                        \
        QUEUE_NEXT(q) = (q); \
        QUEUE_PREV(q) = (q); \
    } while (0)

// add queue n from the tail of queue h
#define QUEUE_ADD(h, n)                     \
    do                                      \
    {                                       \
        QUEUE_PREV_NEXT(h) = QUEUE_NEXT(n); \
        QUEUE_NEXT_PREV(n) = QUEUE_PREV(h); \
        QUEUE_PREV_NEXT(n) = (h);           \
        QUEUE_PREV(h) = QUEUE_PREV(n);      \
    } while (0)

// split the queue h with node q as the split point
// and use the second half to form a new queue n
#define QUEUE_SPLIT(h, q, n)           \
    do                                 \
    {                                  \
        QUEUE_PREV(n) = QUEUE_PREV(h); \
        QUEUE_PREV_NEXT(h) = (n);      \
        QUEUE_NEXT(n) = (q);           \
        QUEUE_PREV(h) = QUEUE_PREV(q); \
        QUEUE_PREV_NEXT(q) = (h);      \
        QUEUE_PREV(q) = (n);           \
    } while (0)

// move queue h to queue n
// if h is empty, then queue n is initialized to an empty queue
// if h is not empty, then add all the nodes in queue h (except the node h)
// into queue n which likes split(h, h->next, n)
#define QUEUE_MOVE(h, n)          \
    do                            \
    {                             \
        if (QUEUE_EMPTY(h))       \
            QUEUE_INIT(n);        \
        else                      \
        {                         \
            q = QUEUE_HEAD(h);    \
            QUEUE_SPLIT(h, q, n); \
        }                         \
    } while (0)

// insert node from the head of queue
#define QUEUE_INSERT_HEAD(h, q)        \
    do                                 \
    {                                  \
        QUEUE_PREV(q) = (h);           \
        QUEUE_NEXT(q) = QUEUE_NEXT(h); \
        QUEUE_NEXT_PREV(h) = (q);      \
        QUEUE_NEXT(h) = (q);           \
    } while (0)

// insert node from the tail of queue
#define QUEUE_INSERT_TAIL(h, q)        \
    do                                 \
    {                                  \
        QUEUE_NEXT(q) = (h);           \
        QUEUE_PREV(q) = QUEUE_PREV(h); \
        QUEUE_PREV_NEXT(h) = (q);      \
        QUEUE_PREV(h) = (q);           \
    } while (0)

// remove a node
#define QUEUE_REMOVE(q)                     \
    do                                      \
    {                                       \
        QUEUE_PREV_NEXT(q) = QUEUE_NEXT(q); \
        QUEUE_NEXT_PREV(q) = QUEUE_PREV(q); \
    } while (0)
