/* Pro-Type Kernel v1.3		*/
/* Scheduler v0.3		*/
/* By LegendMythe		*/

#include <scheduler.h>

uint64_t timer_current_tick = 0;

thread_t *current_thread;
sched_spinlock_t sched_lock;

thread_t *sched_ready_que_high	= 0;
thread_t *sched_ready_que_med	= 0;
thread_t *sched_ready_que_low	= 0;

void tm_sched_add_to_que(thread_t *thread);

/** Set's multithreading up. Creates a current thread structure. **/
void setup_tm(void)
{
	/* Initialise the current thread structure with the kernel thread info. */
	current_thread 				= malloc(sizeof(thread_t));
	current_thread->thid 			= 0x00;
	current_thread->flags 			= THREAD_FLAG_READY |  THREAD_FLAG_KERNEL | THREAD_FLAG_PROCESS;
	current_thread->name			= "Kernel Setup";
	current_thread->priority		= THREAD_PRIORITY_HIGHEST;
	current_thread->quantum  		= 1000;
	current_thread->parent_thid 		= 0;
	current_thread->next			= 0;

	/* Initialise the spinlocks to prevent race conditions. */
	sched_lock.sched_ready_que_high	= SPINLOCK_UNLOCKED;
	sched_lock.sched_ready_que_med		= SPINLOCK_UNLOCKED;
	sched_lock.sched_ready_que_low		= SPINLOCK_UNLOCKED;
	sched_lock.sched_notready_que		= SPINLOCK_UNLOCKED;
}

/** Gets called by the timer routine to swap the current running thread with a new one. **/
uint64_t tm_schedule(uint64_t rsp)
{
	/* Save current possition in the stack of the thread and increase the current tick. */
	current_thread->rsp = rsp;
	timer_current_tick++;

	/* Figure out from which list we should pick the next thread. Every odd current tick is a high priority thread. */
	if(sched_ready_que_high && timer_current_tick%2)
	{
		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_que(current_thread);

		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_que_high) == SPINLOCK_LOCKED);

		/* Change current thread and change the begining of the appropriate list. */
		current_thread 		= sched_ready_que_high;
		sched_ready_que_high 	= sched_ready_que_high->next;

		/* Unlock the spinlock. */
		sched_lock.sched_ready_que_high = SPINLOCK_UNLOCKED;
	}
	/* If current tick is divisable by 6 it's time for a low priority thread. */
	else if(sched_ready_que_low && timer_current_tick%6)
	{
		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_que(current_thread);

		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_que_low) == SPINLOCK_LOCKED);

		/* Change current thread and change the begining of the appropriate list. */
		current_thread 		= sched_ready_que_low;
		sched_ready_que_low 	= sched_ready_que_low->next;

		/* Unlock the spinlock. */
		sched_lock.sched_ready_que_low = SPINLOCK_UNLOCKED;
	}
	/* If the current tick isn't divisable by 6 but is even it's time for a medium priority thread. */
	else if(sched_ready_que_med)
	{
		/* Add the current thread to the end of the correct list. */
		tm_sched_add_to_que(current_thread);

		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_que_med) == SPINLOCK_LOCKED);

		/* Change current thread and change the begining of the appropriate list. */
		current_thread 		= sched_ready_que_med;
		sched_ready_que_med 	= sched_ready_que_med->next;

		/* Unlock the spinlock. */
		sched_lock.sched_ready_que_med = SPINLOCK_UNLOCKED;
	}

	/* Change pointer to the next thread. Return rsp for the actual task switch. */
	current_thread->next = 0;
	return current_thread->rsp;
}


/** Adds the current running thread to the end of the appropriate list. **/
void tm_sched_add_to_que(thread_t *thread)
{
	/* If the current thread has a high priority add it to the end of the high priority thread. */
	if(thread->priority == THREAD_PRIORITY_HIGHEST)
	{
		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_que_high) == SPINLOCK_LOCKED);

		/* Check wether the appropriate list exists. */
		if(sched_ready_que_high)
		{
			/* Itterate through the list till the end. */
			thread_t *itterator = sched_ready_que_high;
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
			sched_ready_que_high = thread;
			thread->next =0;
		}

		/* Unlock the spinlock. */
		sched_lock.sched_ready_que_high = SPINLOCK_UNLOCKED;
	}
	else if(thread->priority == THREAD_PRIORITY_NORMAL)
	{
		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_que_med) == SPINLOCK_LOCKED);

		/* Check wether the appropriate list exists. */
		if(sched_ready_que_med)
		{
			/* Itterate through the list till the end. */
			thread_t *itterator = sched_ready_que_med;
			while(itterator->next)
			{
				itterator=itterator->next;
			}
			itterator->next = thread;
		}
		else
		{
			/* Else: create a list! */
			sched_ready_que_med = thread;
		}

		/* Unlock the spinlock. */
		sched_lock.sched_ready_que_med = SPINLOCK_UNLOCKED;
	}
	else
	{
		/* Attempt to unlock the spinlock on the list. */
		while (spinlock_lock(&sched_lock.sched_ready_que_low) == SPINLOCK_LOCKED);

		/* Check wether the appropriate list exists. */
		if(sched_ready_que_low)
		{
			/* Itterate through the list till the end. */
			thread_t *itterator = sched_ready_que_low;
			while(itterator->next)
			{
				itterator=itterator->next;
			}
			itterator->next = thread;
		}
		else
		{
			/* Else: create a list! */
			sched_ready_que_low = thread;
		}

		/* Unlock the spinlock. */
		sched_lock.sched_ready_que_low = SPINLOCK_UNLOCKED;
	}
}
