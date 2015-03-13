/* Pro-Type Kernel v1.3		*/
/* Spinlocks v0.1			*/
/* By LegendMythe			*/

#include <mutex.h>

static u64int atomic_test_and_set (volatile spinlock_t *lock) 
{
        register spinlock_t value = SPINLOCK_UNLOCKED;
       
        asm volatile("lock\
                      xchg   %0, %1"
                      : "=q" (value), "=m" (*lock)
                      : "0" (value));
       
  return value;
}


void mutex_lock(mutex_t *m)
{
	while (atomic_test_and_set (&m->lock) == SPINLOCK_LOCKED);
	/*{
		ProKillThread();
	}*/
}
void mutex_unlock(mutex_t *m)
{
	m->lock = 0;
}