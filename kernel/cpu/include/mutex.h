/* Pro-Type Kernel v1.3	*/
/* Spinlocks v0.1		*/
/* By LegendMythe		*/

#ifndef MUTEX_H
#define MUTEX_H

#include<common.h>

#define SPINLOCK_LOCKED 0
#define SPINLOCK_UNLOCKED 1

typedef volatile u64int spinlock_t;

typedef struct
{
	spinlock_t lock;
	//thread_list_t *waiting;
} __attribute__((packed)) mutex_t;

void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);

#endif		
