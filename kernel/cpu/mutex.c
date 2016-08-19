/* Pro-Type Kernel v1.3		*/
/* Spinlocks v0.1		*/
/* By LegendMythe		*/

#include <mutex.h>
#include<scheduler.h>

/* TODO: Locked Arthmetic and test functions. */
/* -ie inc, dec, add, or, not, */

void acquireLock(volatile uint32_t *l)
{
	while (__sync_lock_test_and_set (l, 1));
	__sync_synchronize();
}

void releaseLock(volatile uint32_t *l)
{
	__sync_synchronize();
	__sync_lock_release(l);
}

uint64_t spinlock_lock (volatile spinlock_t *lock)
{

        return __sync_lock_test_and_set (lock, 1);
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

void mutex_setup(mutex_t *m)
{
	m->lock 		= 0;
	m->waiting_queue 	= 0;
	m->waiting_queue_lock	= 0;
}

/* NOT yet tested! */
void mutex_lock(mutex_t *m)
{
	while (__sync_lock_test_and_set (&m->lock, 1))
	{
		__sync_synchronize();
		while (__sync_lock_test_and_set (&m->waiting_queue_lock, 1));
		__sync_synchronize();
		if(m->waiting_queue)
		{
			while(m->waiting_queue->next)
			{
				m->waiting_queue = m->waiting_queue->next;
			}
			m->waiting_queue->next = tm_thread_get_current_thread();
		}
		else
		{
			m->waiting_queue = tm_thread_get_current_thread();
		}
		__sync_lock_release(&m->waiting_queue_lock, 1);
		tm_sched_kill_current_thread();
	}
}

void mutex_unlock(mutex_t *m)
{
	__sync_lock_release(&m->lock);
	__sync_synchronize();


	while (__sync_lock_test_and_set (&m->waiting_queue_lock, 1));
	__sync_synchronize();
	if(m->waiting_queue)
	{
		tm_sched_add_to_queue(m->waiting_queue);
		m->waiting_queue = m->waiting_queue->next;
	}
	__sync_lock_release(&m->waiting_queue_lock, 1);
}
