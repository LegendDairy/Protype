/* Pro-Type Kernel v1.3		*/
/* Spinlocks v0.1		*/
/* By LegendMythe		*/

#include <mutex.h>

/* TODO: Locked Arthmetic and test functions. */
/* -ie inc, dec, add, or, not, */

uint64_t spinlock_lock (volatile spinlock_t *lock)
{
        register spinlock_t value = SPINLOCK_UNLOCKED;

        asm volatile("lock\
                      xchg   %0, %1"
                      : "=q" (value), "=m" (*lock)
                      : "0" (value) );

        return value;
}

uint64_t atomic_fetch_add(volatile uint64_t* p, uint64_t incr)
{

     	uint64_t result;
	asm volatile ("lock; xadd %0, %1" :
            "=r"(result), "=m"(*p):
            "0"(incr), "m"(*p) :
            "memory");
    return result;
}

void mutex_lock(mutex_t *m)
{
	while (spinlock_lock (&m->lock) == SPINLOCK_LOCKED);
	/*{
		schedule(); // software interupt and schedule
	}*/
}

void mutex_unlock(mutex_t *m)
{
	m->lock = 0;
	/*
	if(m->waiting)
	{
		ProWakeThread(m->waiting);
		m->waiting = m->waiting->next;
		m->waiting->prev = 0;
	}
	*/
}
