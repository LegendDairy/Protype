/* Pro-Type Kernel v0.2		*/
/* Scheduler v0.3		*/
/* By LegendDairy		*/

#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <thread.h>

#ifdef __cplusplus

class scheduler_c
{
private:
	/* Pointers to threads and threadlists. */
	thread_t *idle_thread;
	thread_t *current_thread;
	thread_t *highest_priority_queue;
	thread_t *medium_priority_queue;
	thread_t *lowest_priority_queue;

	/* Integers: locks and current load */
	uint32_t id;
	uint32_t highest_priority_lock;
	uint32_t medium_priority_lock;
	uint32_t lowest_priority_lock;
	uint32_t load;
	uint32_t current_tick;

public:
	/** Constructor for the scheduler. 							**/
	scheduler_c(uint32_t apic_id, uint32_t bootstrap);
	/** Saves rsp of current running thread, and returns rsp of new thread. 		**/
	uint64_t schedule(uint64_t rsp);
	/** Returns current load on this scheduler, necessary for load balancing. 		**/
	uint32_t get_load(void);
	/** Returns the id of this scheduler.		 					**/
	uint32_t get_id(void);
	/** Adds a give thread to the right queue.	 					**/
	void add_to_queue(thread_t *thread);
	/** Removes a given thread from the queue.						**/
	void remove_from_queue(thread_t *thread);
	/** Stops the excution of the current running thread and removes it from the queue.	**/
	void stop_current_thread(void);
	/** Returns current running thread. 							**/
	thread_t *get_current_thread(void);
};

/** Gets called by the timer routine to swap the current running thread with a new one. 	**/
extern "C" uint64_t tm_schedule(uint64_t);
#endif

/** Intialises multithreading. Creates a current thread structure for the BSP. 			**/
void setup_tm(void);
/** Adds the current running thread to the end of the appropriate list. Return CPU id.		**/
uint32_t tm_sched_add_to_queue(thread_t *thread);
/** Stops excecution of current running thread. 						**/
void tm_sched_kill_current_thread(void);
/** Puts the current running thread to sleep for a given amount of milli seconds. 		**/
void tm_schedule_sleep(uint64_t millis);

#endif
