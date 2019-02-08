/*
 * Null implementation of threading
 */

#include "thread.h"

mstime_t __attribute__((weak)) sem_down(struct semaphore *sem, mstime_t ms)
{
    (void)sem;
    (void)ms;
    return 0;
}

void __attribute__((weak)) sem_up(struct semaphore *sem)
{
    (void)sem;
}

