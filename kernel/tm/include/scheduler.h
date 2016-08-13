/* Pro-Type Kernel v1.3	*/
/* Scheduler v0.3 	*/
/* By LegendMythe       */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include<thread.h>
#include<mutex.h>

/* Structure with all the spinlocks for the scheduler. */
typedef struct
{
	spinlock_t sched_ready_queue_high;
	spinlock_t sched_ready_queue_med;
	spinlock_t sched_ready_queue_low;
	spinlock_t sched_notready_queue;
} sched_spinlock_t;

/** Intialises multithreading. Creates a current thread structure for the BSP. **/
void setup_tm(void);
/** Gets called by the timer routine to swap the current running thread with a new one. **/
#ifdef __cplusplus
extern "C" uint64_t tm_schedule(uint64_t);
#endif

void tm_sched_add_to_queue(thread_t *thread);

#endif
