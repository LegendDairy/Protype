/* Pro-Type Kernel v1.3		*/
/* Scheduler v0.3		*/
/* By LegendMythe		*/

#include <scheduler.h>
#include <apic.h>
#include <acpi.h>


uint64_t timer_current_tick = 0;

thread_t *sched_ready_queue_high	= 0;
thread_t *sched_ready_queue_med	= 0;
thread_t *sched_ready_queue_low	= 0;
sched_spinlock_t sched_lock;
uint64_t sched_big_locky = 0;



/** Set's multithreading up. Creates a current thread structure for kernel setup thread. **/
void setup_tm(void)
{
	/* Itterate through system info structure to find current (BSP) processor. */
	processor_t *current_cpu = system_info_get_current_cpu();
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
	sched_lock.sched_ready_queue_high			= SPINLOCK_UNLOCKED;
	sched_lock.sched_ready_queue_med			= SPINLOCK_UNLOCKED;
	sched_lock.sched_ready_queue_low			= SPINLOCK_UNLOCKED;
	sched_lock.sched_notready_queue			= SPINLOCK_UNLOCKED;

}

/** Gets called by the timer routine to swap the current running thread with a new one from the queue. **/
uint64_t tm_schedule(uint64_t rsp)
{
	acquireLock(&sched_big_locky);
	processor_t *current_cpu	= system_info_get_current_cpu();


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

	current_cpu->timer_current_tick++;

	/* Test if a thread is running on this logical cpu. */
	if(!current_cpu->current_thread)
	{
		if(sched_ready_queue_high)
		{
			/* Attempt to unlock the spinlock on the list. */
			while (spinlock_lock(&sched_lock.sched_ready_queue_high) == SPINLOCK_LOCKED);

			/* Change current thread and change the begining of the appropriate list. */
			current_cpu->current_thread 		= sched_ready_queue_high;
			sched_ready_queue_high 			= sched_ready_queue_high->next;

			/* Unlock the spinlock. */
			sched_lock.sched_ready_queue_high = SPINLOCK_UNLOCKED;
			current_cpu->current_thread->next = 0;
			releaseLock(&sched_big_locky);
			return current_cpu->current_thread->rsp;
		}
		else
		{
			releaseLock(&sched_big_locky);
			return rsp;
		}

	}

	/* Save current possition in the stack of the thread and increase the current tick. */
	current_cpu->current_thread->rsp = rsp;

	/* Figure out from which list we should pick the next thread. Every odd current tick is a high priority thread. */
	if(sched_ready_queue_high && current_cpu->timer_current_tick%2)
	{
		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_queue(current_cpu->current_thread);

		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_queue_high) == SPINLOCK_LOCKED);

		/* Change current thread and change the begining of the appropriate list. */
		current_cpu->current_thread 		= sched_ready_queue_high;
		sched_ready_queue_high 	= sched_ready_queue_high->next;

		/* Unlock the spinlock. */
		sched_lock.sched_ready_queue_high = SPINLOCK_UNLOCKED;
	}
	/* If current tick is divisable by 6 it's time for a low priority thread. */
	else if(sched_ready_queue_low && timer_current_tick%6)
	{
		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_queue(current_cpu->current_thread);

		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_queue_low) == SPINLOCK_LOCKED);

		/* Change current thread and change the begining of the appropriate list. */
		current_cpu->current_thread 		= sched_ready_queue_low;
		sched_ready_queue_low 	= sched_ready_queue_low->next;

		/* Unlock the spinlock. */
		sched_lock.sched_ready_queue_low = SPINLOCK_UNLOCKED;
	}
	/* If the current tick isn't divisable by 6 but is even it's time for a medium priority thread. */
	else if(sched_ready_queue_med)
	{
		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_queue(current_cpu->current_thread);

		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_queue_med) == SPINLOCK_LOCKED);

		/* Change current thread and change the begining of the appropriate list. */
		current_cpu->current_thread 		= sched_ready_queue_med;
		sched_ready_queue_med 	= sched_ready_queue_med->next;

		/* Unlock the spinlock. */
		sched_lock.sched_ready_queue_med = SPINLOCK_UNLOCKED;
	}

	/* Change pointer to the next thread. Return rsp for the actual task switch. */
	current_cpu->current_thread->next = 0;
	releaseLock(&sched_big_locky);
	return current_cpu->current_thread->rsp;
}


/** Adds the current running thread to the end of the appropriate list. **/
void tm_sched_add_to_queue(thread_t *thread)
{
	/* If the current thread has a high priority add it to the end of the high priority thread. */
	if(thread->priority == THREAD_PRIORITY_HIGHEST)
	{
		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_queue_high) == SPINLOCK_LOCKED);

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

		/* Unlock the spinlock. */
		sched_lock.sched_ready_queue_high = SPINLOCK_UNLOCKED;
	}
	else if(thread->priority == THREAD_PRIORITY_NORMAL)
	{
		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_queue_med) == SPINLOCK_LOCKED);

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
		}
		else
		{
			/* Else: create a list! */
			sched_ready_queue_med = thread;
		}

		/* Unlock the spinlock. */
		sched_lock.sched_ready_queue_med = SPINLOCK_UNLOCKED;
	}
	else
	{
		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_queue_low) == SPINLOCK_LOCKED);

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
		}
		else
		{
			/* Else: create a list! */
			sched_ready_queue_low = thread;
		}

		/* Unlock the spinlock. */
		sched_lock.sched_ready_queue_low = SPINLOCK_UNLOCKED;
	}
}
