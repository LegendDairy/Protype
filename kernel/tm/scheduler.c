/* Pro-Type Kernel v1.3		*/
/* Scheduler v0.2		*/
/* By LegendMythe		*/

#include <scheduler.h>

thread_t *sched_ready_queue_high	= 0;
thread_t *sched_ready_queue_med		= 0;
thread_t *sched_ready_queue_low		= 0;
sched_spinlock_t sched_lock;
thread_t *sched_sleep_queue 		= 0;
extern topology_t *system_info;


/** Set's multithreading up. Creates a current thread structure for kernel setup thread. **/
void setup_tm(void)
{
	/* Itterate through system info structure to find current (BSP) processor. */
	register processor_t* current_cpu asm("r12")= system_info_get_current_cpu();
	if(!current_cpu)
	{
		printf("\n[TM]: Error: 0x01: Couldn't find current CPU in system_info structure!");
		for(;;);
	}

	/* Initialise the current thread structure with the kernel thread info. */
	current_cpu->current_thread 			= (thread_t*)malloc(sizeof(thread_t));
	current_cpu->current_thread->thid 		= 0x00;
	current_cpu->current_thread->flags 		= THREAD_FLAG_READY |  THREAD_FLAG_KERNEL | THREAD_FLAG_PROCESS;
	current_cpu->current_thread->name		= "Kernel Setup";
	current_cpu->current_thread->priority		= THREAD_PRIORITY_HIGHEST;
	current_cpu->current_thread->quantum  		= 1000;
	current_cpu->current_thread->parent_thid	= 0;
	current_cpu->current_thread->next		= 0;

	/* Initialise the spinlocks to prevent race conditions. */
	sched_lock.sched_ready_queue_high		= SPINLOCK_UNLOCKED;
	sched_lock.sched_ready_queue_med		= SPINLOCK_UNLOCKED;
	sched_lock.sched_ready_queue_low		= SPINLOCK_UNLOCKED;
	sched_lock.sched_notready_queue			= SPINLOCK_UNLOCKED;

}

/** Gets called by the timer routine to swap the current running thread with a new one from the queue. **/
uint64_t tm_schedule(uint64_t rsp)
{
	register processor_t *current_cpu asm("r12") = system_info->cpu_list;
	while(current_cpu && (!((uint32_t)current_cpu->apic_id == lapic_read(apic_reg_id) >> 24)))
	{
		current_cpu = current_cpu->next;
	}

	/* Increase schedule-counter. */
	current_cpu->timer_current_tick++;

	/* TODO:
	char fxsave_region[512] __attribute__((aligned(16)));
	asm volatile(" fxsave; "::"m"(fxsave_region));
	asm volatile(" fxstor; "::"m"(fxsave_region));
	*/

	/* Test if a correct cpu structure exists. */
	if(!current_cpu)
	{
		printf("error");
		for(;;);
	}

	/* Test if a thread is running on this logical cpu. If so we must save it's progress. */
	if(current_cpu->current_thread)
	{
		/* Save current possition in the stack of the thread. */
		current_cpu->current_thread->rsp = rsp;
		if(current_cpu->current_thread->flags & THREAD_FLAG_STOPPED)
		{
			current_cpu->current_thread = 0;
		}
	}
	else
	{
		current_cpu->idle_thread->rsp = rsp;
	}


	/* Attempt to unlock the spinlock on the list. */
	acquireLock(&sched_lock.sched_ready_queue_high);
	if(sched_ready_queue_high)
	{

		/* Change current thread and change the begining of the appropriate list. */
		thread_t *tmp 				= current_cpu->current_thread;
		current_cpu->current_thread 		= sched_ready_queue_high;
		sched_ready_queue_high 			= sched_ready_queue_high->next;

		/* Unlock the spinlock. */
		releaseLock(&sched_lock.sched_ready_queue_high);

		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_queue(tmp);
		return current_cpu->current_thread->rsp;
	}
	else
	releaseLock(&sched_lock.sched_ready_queue_high);
	acquireLock(&sched_lock.sched_ready_queue_med);
	if(sched_ready_queue_low  && current_cpu->timer_current_tick%6)
	{
		/* Change current thread and change the begining of the appropriate list. */
		thread_t *tmp 				= current_cpu->current_thread;
		current_cpu->current_thread 		= sched_ready_queue_low;
		sched_ready_queue_low 			= sched_ready_queue_low->next;

		/* Unlock the spinlock. */
		releaseLock(&sched_lock.sched_ready_queue_med);

		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_queue(tmp);
		return current_cpu->current_thread->rsp;
	}
	else
	releaseLock(&sched_lock.sched_ready_queue_med);
	acquireLock(&sched_lock.sched_ready_queue_low);
	if(sched_ready_queue_med)
	{

		/* Change current thread and change the begining of the appropriate list. */
		thread_t *tmp 				= current_cpu->current_thread;
		current_cpu->current_thread 		= sched_ready_queue_med;
		sched_ready_queue_med 			= sched_ready_queue_med->next;

		/* Unlock the spinlock. */
		releaseLock(&sched_lock.sched_ready_queue_low);

		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_queue(tmp);
		return current_cpu->current_thread->rsp;
	}

	releaseLock(&sched_lock.sched_ready_queue_low);

	if(current_cpu->current_thread)
	{
		return current_cpu->current_thread->rsp;
	}
	else
	{
		return current_cpu->idle_thread->rsp;
	}
}

void tm_sched_kill_current_thread(void)
{
	asm volatile ("cli");

	register processor_t *current_cpu asm("r12") = system_info_get_current_cpu();
	current_cpu->current_thread->flags |= THREAD_FLAG_STOPPED;

	asm volatile ("sti");
	asm volatile("int $33");
	while(1);
}

/** Adds the current running thread to the end of the appropriate list. **/
void tm_sched_add_to_queue(thread_t *thread)
{
	if(thread)
	{
		lapic_write(0x80, 0xFF);
		/* If the current thread has a high priority add it to the end of the high priority thread. */
		if(thread->priority == THREAD_PRIORITY_HIGHEST)
		{
			acquireLock(&sched_lock.sched_ready_queue_high);
			__sync_synchronize();
			/* Check wether the appropriate list exists. */
			if(sched_ready_queue_high)
			{
				/* Itterate through the list till the end. */
				thread_t *itterator = sched_ready_queue_high;
				while(itterator->next)
				{
					itterator=itterator->next;
				}
				itterator->next = thread;
				thread->next =0;

			}
			else
			{
				/* Else: create a list! */
				sched_ready_queue_high = thread;
				thread->next =0;
			}
			releaseLock(&sched_lock.sched_ready_queue_high);
		}
		else if(thread->priority == THREAD_PRIORITY_NORMAL)
		{
			acquireLock(&sched_lock.sched_ready_queue_med);
			__sync_synchronize();
			/* Check wether the appropriate list exists. */
			if(sched_ready_queue_med)
			{
				/* Itterate through the list till the end. */
				thread_t *itterator = sched_ready_queue_med;
				while(itterator->next)
				{
					itterator=itterator->next;
				}
				itterator->next = thread;
				thread->next =0;
			}
			else
			{
				/* Else: create a list! */
				sched_ready_queue_med = thread;
				thread->next =0;
			}
			releaseLock(&sched_lock.sched_ready_queue_med);

		}
		else
		{
			acquireLock(&sched_lock.sched_ready_queue_low);
			__sync_synchronize();
			/* Check wether the appropriate list exists. */
			if(sched_ready_queue_low)
			{
				/* Itterate through the list till the end. */
				thread_t *itterator = sched_ready_queue_low;
				while(itterator->next)
				{
					itterator=itterator->next;
				}
				itterator->next = (thread_t *)thread;
				thread->next =0;
			}
			else
			{
				/* Else: create a list! */
				sched_ready_queue_low = thread;
				thread->next =0;
			}
			releaseLock(&sched_lock.sched_ready_queue_low);
		}
		lapic_write(0x80, 0x00);
	}
}

uint32_t sleep_lock = 0;
/* Puts the current running thread to sleep for a given amount of milli seconds. */
void tm_schedule_sleep(uint64_t millis)
{
	asm volatile("cli");
	lapic_write(0x80, 0xFF);
	acquireLock(&sleep_lock);

	register processor_t *cpu asm("r12") = system_info_get_current_cpu();

	if(sched_sleep_queue)
	{
		thread_t *iterator 	= (thread_t *)sched_sleep_queue;
		thread_t *prev 		= 0;

		while(iterator->next)
		{
			if(millis < iterator->delta_time)
			{
				cpu->current_thread->delta_time 	= millis;
				if(prev)
				{
					prev->next 			= cpu->current_thread;
				}
				else
				{
					sched_sleep_queue = cpu->current_thread;
				}
				cpu->current_thread->next 		= iterator;

				while(iterator)
				{
					iterator->delta_time 		-= millis;
					iterator 			= iterator->next;
				}
				cpu->current_thread->flags		|= THREAD_FLAG_STOPPED;

				releaseLock(&sleep_lock);
				lapic_write(0x80, 0x00);
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
			cpu->current_thread->delta_time 		= millis;
			if(prev)
			{
				prev->next 				= cpu->current_thread;
			}
			else
			{
				sched_sleep_queue 			= cpu->current_thread;
			}
			cpu->current_thread->next 			= iterator;
			iterator->delta_time 				-= millis;
			cpu->current_thread->flags			|= THREAD_FLAG_STOPPED;

			releaseLock(&sleep_lock);
			lapic_write(0x80, 0x00);
			asm volatile("sti");
			asm volatile("int $33");
			return;
		}
		if(millis >= iterator->delta_time)
		{
		cpu->current_thread->delta_time 			= millis - iterator->delta_time;
		iterator->next 						= cpu->current_thread;
		cpu->current_thread->next = 0;
		cpu->current_thread->flags				|= THREAD_FLAG_STOPPED;

		releaseLock(&sleep_lock);
		asm volatile("sti");
		lapic_write(0x80, 0x00);
		asm volatile("int $33");
		return;
		}
	}
	else
	{
		sched_sleep_queue 					= (thread_t *)cpu->current_thread;
		cpu->current_thread->delta_time 			= millis;
		cpu->current_thread->next 				= 0;
		cpu->current_thread->flags				|= THREAD_FLAG_STOPPED;

		releaseLock(&sleep_lock);
		asm volatile("sti");
		lapic_write(0x80, 0x00);
		asm volatile("int $33");
		return;
	}
}
