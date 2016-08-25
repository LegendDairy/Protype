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
	queue_c *highest_priority_queue;
	queue_c *medium_priority_queue;
	queue_c *lowest_priority_queue;

	/* Integers: locks and current load */
	uint32_t id;
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
	void add_thread(thread_t *thread);
	/** Removes a given thread from the queue.						**/
	void remove_thread(thread_t *thread);
	/** Stops the excution of the current running thread and removes it from the queue.	**/
	void stop_current_thread(void);
	/** Returns current running thread. 							**/
	thread_t *get_current_thread(void);
	/** Decrease the load of the scheduler.							**/
	void decrease_load(void);
	/** Increase load counter of the scheduler.						**/
	void increase_load(void);
};

class queue_c
{
private:
	thread_t *first, *last;
	uint32_t lock;

public:
	/** Thread queue constructor.						**/
	queue_c(void);
	/** Add a thread to the queue. 						**/
	void enqueue(thread_t *);
	/** Get a thread from the queue. 					**/
	thread_t *dequeue(void);
	/** Test if a queue is empty, returns TRUE if not empty. 		**/
	bool not_empty(void);
	/** Finds and removes a thread from the queue.				**/
	void remove_from_queue(thread_t *thread);

};

/** Gets called by the timer routine to swap the current running thread with a new one. 	**/
extern "C" uint64_t tm_schedule(uint64_t);
#endif

#endif
