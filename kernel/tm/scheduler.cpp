/* Pro-Type Kernel v0.2		*/
/* Scheduler v0.3		*/
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

thread_t *sched_sleep_queue 			= 0;
uint32_t sleep_lock				= 0;

scheduler_c::scheduler_c(uint32_t apic_id, uint32_t bootstrap)
{
	id 					= apic_id;
	highest_priority_lock			= 0;
	medium_priority_lock			= 0;
	lowest_priority_lock			= 0;
	load					= 0;
	idle_thread				= tm_thread_create_idle_thread();
	current_tick				= 0;
	highest_priority_queue			= 0;
	medium_priority_queue			= 0;
	lowest_priority_queue			= 0;

	if(apic_id == bootstrap)
	{
		current_thread 			= (thread_t*)malloc(sizeof(thread_t));
		current_thread->thid 		= 0x00;
		current_thread->flags 		= THREAD_FLAG_READY |  THREAD_FLAG_KERNEL;
		current_thread->name		= "Kernel Setup";
		current_thread->priority	= THREAD_PRIORITY_HIGHEST;
		current_thread->quantum  	= 10;
		current_thread->parent_thid	= 0;
		current_thread->next		= 0;
	}
	else
	{
		current_thread			= 0;
	}
}

uint64_t scheduler_c::schedule(uint64_t rsp)
{
	/* Increase schedule-counter. */
	current_tick++;
	/* Test if a thread is running on this logical cpu. If so we must save it's progress. */
	if(current_thread)
	{
		/* Save current possition in the stack of the thread. */
		current_thread->rsp = rsp;

		if(current_thread->flags & THREAD_FLAG_STOPPED)
		{
			load--;
			current_thread = 0;
		}

	}
	else
	{
		idle_thread->rsp = rsp;
	}

	/* Attempt to unlock the spinlock on the list. */
	acquireLock(&highest_priority_lock);
	if(highest_priority_queue && current_tick%2)
	{

		/* Change current thread and change the begining of the appropriate list. */
		thread_t *tmp 				= current_thread;
		current_thread 				= highest_priority_queue;
		highest_priority_queue 			= highest_priority_queue->next;

		/* Unlock the spinlock. */
		releaseLock(&highest_priority_lock);

		/* Add the current thread to the end of the correct list. */
		add_to_queue(tmp);
		return current_thread->rsp;
	}
	else
	releaseLock(&highest_priority_lock);
	acquireLock(&lowest_priority_lock);
	if(lowest_priority_queue  && current_tick%6)
	{
		/* Change current thread and change the begining of the appropriate list. */
		thread_t *tmp 				= current_thread;
		current_thread 				= lowest_priority_queue;
		lowest_priority_queue 			= lowest_priority_queue->next;

		/* Unlock the spinlock. */
		releaseLock(&lowest_priority_lock);

		/* Add the current thread to the end of the correct list. */
		add_to_queue(tmp);
		return current_thread->rsp;
	}
	else
	releaseLock(&lowest_priority_lock);
	acquireLock(&medium_priority_lock);
	if(medium_priority_queue)
	{
		/* Change current thread and change the begining of the appropriate list. */
		thread_t *tmp 				= current_thread;
		current_thread 				= medium_priority_queue;
		medium_priority_queue 			= medium_priority_queue->next;

		/* Unlock the spinlock. */
		releaseLock(&medium_priority_lock);

		/* Add the current thread to the end of the correct list. */
		add_to_queue(tmp);
		return current_thread->rsp;
	}
	else
	releaseLock(&medium_priority_lock);

	/* No new thread was found! Continue current thread or idle thread. */
	if(current_thread)
	{
		return current_thread->rsp;
	}
	else
	{
		return idle_thread->rsp;
	}
}

void scheduler_c::add_to_queue(thread_t *thread)
{
	if(thread)
	{
		uint32_t priority = lapic_read(0x80);
		lapic_write(0x80, 0xFF);
		load++;
		/* If the current thread has a high priority add it to the end of the high priority thread. */
		if(thread->priority == THREAD_PRIORITY_HIGHEST)
		{
			acquireLock(&highest_priority_lock);
			/* Check wether the appropriate list exists. */
			if(highest_priority_queue)
			{
				/* Itterate through the list till the end. */
				thread_t *iterator = highest_priority_queue;
				while(iterator->next)
				{
					iterator=iterator->next;
				}
				iterator->next = thread;
				thread->next 	= 0;

			}
			else
			{
				/* Else: create a list! */
				highest_priority_queue 	= thread;
				thread->next 		= 0;
			}
			releaseLock(&highest_priority_lock);
		}
		else if(thread->priority == THREAD_PRIORITY_NORMAL)
		{
			acquireLock(&medium_priority_lock);
			/* Check wether the appropriate list exists. */
			if(medium_priority_queue)
			{
				/* Itterate through the list till the end. */
				thread_t *iterator = medium_priority_queue;
				while(iterator->next)
				{
					iterator=iterator->next;
				}
				iterator->next = thread;
				thread->next 	= 0;

			}
			else
			{
				/* Else: create a list! */
				medium_priority_queue 	= thread;
				thread->next 		= 0;
			}
			releaseLock(&medium_priority_lock);

		}
		else
		{
			acquireLock(&lowest_priority_lock);
			/* Check wether the appropriate list exists. */
			if(lowest_priority_queue)
			{
				/* Itterate through the list till the end. */
				thread_t *iterator = lowest_priority_queue;
				while(iterator->next)
				{
					iterator=iterator->next;
				}
				iterator->next = thread;
				thread->next 	= 0;

			}
			else
			{
				/* Else: create a list! */
				lowest_priority_queue 	= thread;
				thread->next 		= 0;
			}
			releaseLock(&lowest_priority_lock);
		}
		lapic_write(0x80, priority);
	}
}

void scheduler_c::stop_current_thread(void)
{
	asm volatile ("cli");

	current_thread->flags |= THREAD_FLAG_STOPPED;

	asm volatile ("sti");
	asm volatile("int $33");
	while(1);
}

thread_t *scheduler_c::get_current_thread(void)
{
	return current_thread;
}

void scheduler_c::set_current_thread(thread_t *thread)
{
	current_thread = thread;
}

void scheduler_c::remove_from_queue(thread_t *thread)
{

}

uint32_t scheduler_c::get_load(void)
{
	return load;
}

uint32_t scheduler_c::get_id(void)
{
	return id;
}

/** Stops excecution of current running thread. **/
void tm_sched_kill_current_thread(void)
{
	system_c *system = system_c::get_instance();
	register scheduler_c *scheduler = system->get_current_scheduler();
	scheduler->stop_current_thread();
}

/** Gets called by the timer routine to swap the current running thread with a new one from the queue. **/
uint64_t tm_schedule(uint64_t rsp)
{
	register system_c *system = system_c::get_instance();
	register scheduler_c *scheduler = system->get_current_scheduler();
	return scheduler->schedule(rsp);
}

/* Adds a thread to the cpu with the lowest load. */
uint32_t tm_sched_add_to_queue(thread_t *thread)
{
	system_c *system = system_c::get_instance();
	cpu_c *iterator = system->get_cpu_list();
	cpu_c *lowest = iterator;

	while(iterator)
	{
		if(iterator->scheduler->get_load() < lowest->scheduler->get_load())
			lowest = iterator;

		iterator = iterator->next;
	}

	lowest->scheduler->add_to_queue(thread);
	return lowest->get_id();
}

/* Puts the current running thread to sleep for a given amount of milli seconds. */
void tm_schedule_sleep(uint64_t millis)
{
	asm volatile("cli");
	uint32_t tpr = lapic_read(0x80);
	lapic_write(0x80, 0xFF);
	acquireLock(&sleep_lock);

	system_c *system = system_c::get_instance();
	register scheduler_c *sched = system->get_current_scheduler();

	if(sched_sleep_queue)
	{
		thread_t *iterator 	= (thread_t *)sched_sleep_queue;
		thread_t *prev 		= 0;

		while(iterator->next)
		{
			if(millis < iterator->delta_time)
			{
				sched->get_current_thread()->delta_time 	= millis;
				if(prev)
				{
					prev->next 			= sched->get_current_thread();
				}
				else
				{
					sched_sleep_queue 		= sched->get_current_thread();
				}
				sched->get_current_thread()->next 		= iterator;

				while(iterator)
				{
					iterator->delta_time 		-= millis;
					iterator 			= iterator->next;
				}
				sched->get_current_thread()->flags		|= THREAD_FLAG_STOPPED;

				releaseLock(&sleep_lock);
				lapic_write(0x80, tpr);
				asm volatile("sti");
				asm volatile("int $33");
				return;
			}

			millis 						-= iterator->delta_time;
			prev 						= iterator;
			iterator 					= iterator->next;
		}

		if(millis < iterator->delta_time)
		{
			sched->get_current_thread()->delta_time 		= millis;
			if(prev)
			{
				prev->next 				= sched->get_current_thread();
			}
			else
			{
				sched_sleep_queue 			= sched->get_current_thread();
			}
			sched->get_current_thread()->next 			= iterator;
			iterator->delta_time 				-= millis;
			sched->get_current_thread()->flags			|= THREAD_FLAG_STOPPED;

			releaseLock(&sleep_lock);
			lapic_write(0x80, tpr);
			asm volatile("sti");
			asm volatile("int $33");
			return;
		}
		if(millis >= iterator->delta_time)
		{
		sched->get_current_thread()->delta_time 			= millis - iterator->delta_time;
		iterator->next 						= sched->get_current_thread();
		sched->get_current_thread()->next = 0;
		sched->get_current_thread()->flags				|= THREAD_FLAG_STOPPED;

		releaseLock(&sleep_lock);
		asm volatile("sti");
		lapic_write(0x80, tpr);
		asm volatile("int $33");
		return;
		}
	}
	else
	{
		sched_sleep_queue 					= (thread_t *)sched->get_current_thread();
		sched->get_current_thread()->delta_time 			= millis;
		sched->get_current_thread()->next 				= 0;
		sched->get_current_thread()->flags				|= THREAD_FLAG_STOPPED;

		releaseLock(&sleep_lock);
		asm volatile("sti");
		lapic_write(0x80, tpr);
		asm volatile("int $33");
		return;
	}
}
