/* Pro-Type Kernel v1.3	*/
/* Spinlocks v0.1	*/
/* By LegendMythe	*/

#ifndef MUTEX_H
#define MUTEX_H

#include<common.h>

#define SPINLOCK_LOCKED 0
#define SPINLOCK_UNLOCKED 1

typedef volatile uint64_t spinlock_t;

typedef struct
{
	spinlock_t lock /* = 0 */;
	//thread_list_t *waiting;
} mutex_t;

/** Tests and locks a given spinlock. 		**/
void mutex_lock(mutex_t *m);
/** Unlock a given spinlock. 			**/
void mutex_unlock(mutex_t *m);

#endif
