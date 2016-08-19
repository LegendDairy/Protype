/* Pro-Type Kernel v1.3	*/
/* Scheduler v0.2	*/
/* By LegendMythe       */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <thread.h>

/** Intialises multithreading. Creates a current thread structure for the BSP. **/
void setup_tm(void);
/** Adds the current running thread to the end of the appropriate list. **/
void tm_sched_add_to_queue(thread_t *thread);
/** Stops excecution of current running thread. **/
void tm_sched_kill_current_thread(void);

/** Gets called by the timer routine to swap the current running thread with a new one. **/
#ifdef __cplusplus
extern "C" uint64_t tm_schedule(uint64_t);
#endif

#endif
