/* Pro-Type Kernel v0.2		*/
/* Scheduler Class v0.3		*/
/* By Legenddairy		*/

/* Roadmap:					*/
/* v0.1: simple scheduler			*/
/* v0.2: simple scheduler ported to smp		*/
/* v0.3: scheduler became more scalable 	*/

#include <scheduler.hpp>
#include <system.hpp>
#include <heap.h>
#include <mutex.h>
#include <apic.h>
#include <acpi.h>

/** Thread queue constructor 						**/
queue_c::queue_c(void)
{
	first 	= 0;
	last 	= 0;
	lock 	= 0;
}

/** Get a thread from the queue. 					**/
thread_t *queue_c::dequeue(void)
{
	/* Acquire the spinlock for this thread. */
	acquireLock(&lock);

	/* Is there a queue? */
	if(first)
	{
		/* Take first entry and adjust pointer to the next entry. */
		thread_t tmp = first;
		first = first->next;

		/* Is there a queue left? */
		if(!first)
			last = 0

		/* Release lock and return pointer. */
		releaseLock(&lock);
		return tmp;
	}
	else
	{
		/* There's no queue! */
		releaseLock(&lock);
		return NULL;
	}

}

/** Get a thread from the queue. 					**/
void queue_c::enqueue(thread_t *thread)
{
	/* Acquire the spinlock for this thread. */
	acquireLock(&lock);

	/* Is there a queue? */
	if(first)
	{
		/* Add the new entry to the end of the queue. */
		tread->next = 0;
		last->next = thread;
		last = last->next;
	}
	else
	{
		/* No queue, make one. */
		first = thread;
		last = thread;
	}

	/* Release the spinlock. */
	releaseLock(&lock);

}

/** Test if a queue is empty, returns TRUE if not empty. 		**/
bool queue_c::not_empty(void)
{
	/* Acquire the spinlock for this thread. */
	acquireLock(&lock);

	/* Is there a queue? */
	if(first)
	{
		releaseLock(&lock);
		return TRUE;
	}
	else
	{
		releaseLock(&lock);
		return FALSE;
	}
}

/** Finds and removes a thread from the queue.				**/
void queue::remove_thread(thread_t *thread)
{
	/* Temporary pointers to iterate. */
	thread_t *iterator = first;
	thread_t *prev	   = 0;

	/* Iterate through the list till we find it */
	while(iterator && iterator != thread)
	{
		prev = iterator;
		iterator = iterator->next;
	}

	/* Did we find it? */
	if(iterator)
	{
		/* Is it the first entry? */
		if(prev)
		{
			prev->next = iterator->next;
		}
		else
		{
			first = iterator->next;
		}

		/* Was our thread the end of the queue? */
		if(!iterator->next)
			last = prev;
	}
}

/** Constructor for the scheduler. 							**/
scheduler_c::scheduler_c(uint32_t apic_id, uint32_t bootstrap)
{
	id 					= apic_id;
	load					= 0;
	idle_thread				= tm_thread_create_idle_thread();
	current_tick				= 0;
	highest_priority_queue			= new queue_c();
	medium_priority_queue			= new queue_c();
	lowest_priority_queue			= new queue_c();

	/* Is this the bootstrap scheduler? */
	if(apic_id == bootstrap)
	{
		/* Create an entry for the kernel thread. */
		current_thread 			= (thread_t*)malloc(sizeof(thread_t));
		current_thread->thid 		= 0x00;
		current_thread->flags 		= THREAD_FLAG_READY |  THREAD_FLAG_KERNEL;
		current_thread->name		= "Kernel Setup";
		current_thread->priority	= THREAD_PRIORITY_HIGHEST;
		current_thread->quantum  	= 10;
		current_thread->parent_thid	= 0;
		current_thread->next		= 0;
		load++;
	}
	else
	{
		current_thread			= 0;
	}
}

/** Saves rsp of current running thread, and returns rsp of new thread. 		**/
uint64_t scheduler_c::schedule(uint64_t rsp)
{
	/* Test if a thread is running on this logical cpu. If so we must save it's progress. */
	if(current_thread)
	{
		/* Save current possition in the stack of the thread. */
		current_thread->rsp = rsp;
		/*if(current_thread->next)
		{
			current_thread = current_thread->next;
			return current_thread->rsp;
		}*/
		/* Test if this thread needs to be stopped. */
		if(current_thread->flags & THREAD_FLAG_STOPPED)
		{
			/** Load gets decreased. **/
			load--;
			//free(current_thread);
			/** make sure this thread wont be added.	*/
			current_thread = 0;
		}
	}
	else
	{
		/* Save idle thread RSP */
		idle_thread->rsp = rsp;
	}

	/* Increase schedule-counter. */
	current_tick++;

	/* Every odd tick: a highest priority thread. 			*/
	if(current_tick%2 && highest_priority_queue->not_empty())
	{
		/* Add current thread to a queue, and pop a new thread. */
		add_thread(current_thread);
		current_thread 	= highest_priority_queue->dequeue();
		return current_thread->rsp;
	} /* Every 6th tick: a lowest priority thread. 			*/
	else if(!(current_tick%6) && lowest_priority_queue->not_empty())
	{
		/* Add current thread to a queue, and pop a new thread. */
		add_thread(current_thread);
		current_thread 	= lowest_priority_queue->dequeue();
		return current_thread->rsp;
	} /* Every 2nd and 4th tick: a medium priority thread. 		*/
	else if(medium_priority_queue->not_empty())
	{
		/* Add current thread to a queue, and pop a new thread. */
		add_thread(current_thread);
		current_thread 	= medium_priority_queue->dequeue();
		return current_thread->rsp;
	}/* We didn't find a new thread to run, try to continu. 	*/
	else if(current_thread)
	{
		return current_thread->rsp;
	}/* We can't continu, we ran out of threads; run idle thread.	*/
	else
	{
		return idle_thread->rsp;
	}
}

/** Adds a give thread to the right queue.	 					**/
void scheduler_c::add_thread(thread_t *thread)
{
	if(thread)
	{
		/* Save interrupt state and dissable interrupts. */
		uint32_t priority = lapic_read(0x80);
		lapic_write(0x80, 0xFF);


		if(thread->priority == THREAD_PRIORITY_HIGHEST)
		{
			/** Add to the highest priority aueue. */
			highest_priority_queue->enqueue(thread);
		}
		else if(thread->priority == THREAD_PRIORITY_NORMAL)
		{
			/** Add to the medium priority aueue. */
			medium_priority_queue->enqueue(thread);
		}
		else
		{
			/** Add to the lowest priority aueue. */
			lowest_priority_queue->enqueue(thread);
		}

		/* Restore interupt state */
		lapic_write(0x80, priority);
	}
}

/** Stops the excution of the current running thread and removes it from the queue.	**/
void scheduler_c::stop_current_thread(void)
{
	/* Save interrupt state and dissable interrupts. */
	uint32_t priority = lapic_read(0x80);
	lapic_write(0x80, 0xFF);

	/* Mark the thread as 'stopped'. */
	current_thread->flags |= THREAD_FLAG_STOPPED;

	/* Restore interrupt state */
	lapic_write(0x80, priority);

	/* Yield over controll */
	asm volatile("int $33");

	/* Safety loop. */
	while(1);
}

/** Returns current running thread. 							**/
thread_t *scheduler_c::get_current_thread(void)
{
	return current_thread;
}

/** Removes a given thread from the queue.						**/
void scheduler_c::remove_thread(thread_t *thread)
{
	if(thread)
	{
		/* Save interrupt state and dissable interrupts. */
		uint32_t priority = lapic_read(0x80);
		lapic_write(0x80, 0xFF);


		if(thread->priority == THREAD_PRIORITY_HIGHEST)
		{
			/** Remove this thread from the highest priority aueue. */
			highest_priority_queue->remove_from_queue(thread);
		}
		else if(thread->priority == THREAD_PRIORITY_NORMAL)
		{
			/** Remove this thread from the medium priority aueue. */
			medium_priority_queue->remove_from_queue(thread);
		}
		else
		{
			/** Remove this thread from the lowest priority aueue. */
			lowest_priority_queue->remove_from_queue(thread);
		}

		/* Restore interupt state */
		lapic_write(0x80, priority);
	}
}

/** Returns current load on this scheduler, necessary for load balancing. 		**/
uint32_t scheduler_c::get_load(void)
{
	return load;
}

/** Increase load counter of the scheduler.						**/
void scheduler_c::increase_load(void)
{
	load++;
}

/** Decrease the load of the scheduler.							**/
void scheduler_c::decrease_load(void)
{
	load--;
}

/** Returns the id of this scheduler.		 					**/
uint32_t scheduler_c::get_id(void)
{
	return id;
}

/** Stops excecution of current running thread. **/
void tm_sched_kill_current_thread(void)
{
	system_c::get_current_scheduler()->stop_current_thread();
}
