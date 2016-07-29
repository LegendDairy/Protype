/* Pro-Type Kernel v1.3		*/
/* Spinlocks v0.1		*/
/* By LegendMythe		*/

#include <mutex.h>

uint64_t spinlock_lock (volatile spinlock_t *lock)
{
        register spinlock_t value = SPINLOCK_UNLOCKED;

        asm volatile("lock\
                      xchg   %0, %1"
                      : "=q" (value), "=m" (*lock)
                      : "0" (value) );

        return value;
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
