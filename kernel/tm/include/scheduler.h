/* Pro-Type Kernel v1.3	*/
/* Scheduler v0.3 	*/
/* By LegendMythe       */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include<thread.h>
#include<mutex.h>

/* Structure with all the spinlocks for the scheduler. */
typedef
{
	spinlock_t sched_ready_que_high;
	spinlock_t sched_ready_que_med;
	spinlock_t sched_ready_que_low;
	spinlock_t sched_notready_que;
} sched_spinlock_t;

/** Set's multithreading up. Creates a current thread structure. **/
void setup_tm(void);
/** Gets called by the timer routine to swap the current running thread with a new one. **/
uint64_t tm_schedule(uint64_t rsp);

#endif
