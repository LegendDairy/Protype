/* Pro-Type Kernel v0.2		*/
/* Spinlocks v0.1		*/
/* By LegendDairy		*/

#ifndef MUTEX_H
#define MUTEX_H

#include<common.h>
#include<thread.h>

void acquireLock(volatile uint32_t *l);
void releaseLock(volatile uint32_t *l);

#define SPINLOCK_LOCKED 	1
#define SPINLOCK_UNLOCKED 	0

typedef volatile uint32_t spinlock_t;

typedef struct
{
	spinlock_t lock /* = 0 */;
	thread_t *waiting_queue;
	uint32_t waiting_queue_lock; /* private*/
} mutex_t;

/** Intialises a mutex.				**/
void mutex_setup(mutex_t *m);
/** Tests and locks a given spinlock. 		**/
void mutex_lock(mutex_t *m);
/** Unlock a given spinlock. 			**/
void mutex_unlock(mutex_t *m);
/** Lock a spinlock wo going to sleep if locked. */
uint64_t spinlock_lock(volatile spinlock_t *lock);
uint64_t atomic_fetch_add(volatile uint64_t* p, uint64_t incr);


#endif
