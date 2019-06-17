/* Pro-Type Kernel v0.2		*/
/* Thread Manager v0.1		*/
/* By LegendDairy		*/

#include <thread.h>
#include <scheduler.hpp>
#include <system.hpp>
#include <acpi.h>
#include <apic.h>
#include <heap.h>



/* TODO: Make delta queue class. */
thread_t *sched_sleep_queue 	= 0;
uint32_t sleep_lock		= 0;
uint64_t tm_current_thid 	= 0;
thread_t *not_ready_queue 	= 0;
uint32_t not_ready_lock		= 0;


static void thread_exit(void);
static void tm_idle_thread_fn(void);

/** Creates a new thread and adds it to the not-ready queue. Returns Thread id. 			**/
uint64_t tm_thread_create(fn_t fn, uint64_t argn, char *argv[], uint64_t PLM4T, uint8_t priority, uint64_t quantum, const char *name, uint32_t flags, uint64_t *stack, uint8_t ds, uint8_t cs, uint8_t ss)
{
	/* TODO: Add kernel stack per thread for Usermode support! */
	/* Create and initialise an entry thread structure. */
	thread_t *entry = (thread_t*)malloc(sizeof(thread_t));
	entry->next	= 0;
	entry->thid	= __sync_add_and_fetch(&tm_current_thid, 1);
	entry->name	= name;
	entry->flags	= flags;
	entry->quantum	= quantum;
	entry->priority	= priority;
	entry->parent	= tm_thread_get_current_thread_thid();

	/* Prepare the thread stack. Set the intial register values. */
	*--stack	= (uint64_t)&thread_exit;
	*--stack 	= (uint64_t)ss;			// Stack segment selector
	uint64_t usrrsp = (uint64_t)stack + 8;		// Pointer to Useresp
	*--stack 	= (uint64_t)((uint64_t)usrrsp);	// Pointer to stack
	*--stack 	= (uint64_t)0x200; 		// Interrupts enabled
	*--stack 	= (uint64_t)cs; 		// Code segment selector
	*--stack 	= (uint64_t)fn; 		// RIP

	stack 		-= 2;
	*--stack	= 0; 				// TSPR
	stack 		-= 10;				// Make room for the GPR on the stack.
							// Make room for the GPR on the stack.
	*--stack 	= 0;				// rbp
	*--stack 	= (uint64_t)argn;		// rdi
	*--stack 	= (uint64_t)argv;		// rsi

	*--stack 	= (uint64_t)ds;			// Setup data segment
	*--stack 	= (uint64_t)PLM4T;		// Setup PLM4T for this thread
	entry->rsp	= (uint64_t)stack;		// pointer to the stack

	/* Add the thread to the not ready queue. */
	acquireLock(&not_ready_lock);

	if(not_ready_queue)
	{
		/* Iterate the queue till the end. */
		thread_t *iterator = not_ready_queue;

		while(iterator->next)
			iterator = iterator->next;

		/* Add new thread to the queue */
		iterator->next = entry;
	}
	else
	{
		/* Create a not ready queue. */
		not_ready_queue = entry;
	}

	releaseLock(&not_ready_lock);

	tm_thread_start(entry->thid);

	return entry->thid;
}

/** Searches the not ready queue for a thread with the given thid. 					**/
uint64_t tm_thread_start(uint64_t thid)
{
	/* Pointers to iterate through the list. */
	register thread_t *iterator 	= not_ready_queue;
	register thread_t *prev 	= 0;

	/* Iterate through the not ready queue to find the desired thread. */
	while(iterator && iterator->thid != thid)
	{
		prev = iterator;
		iterator = iterator->next;
	}

	/* Did we find the desired thread? */
	if(iterator)
	{
		/* If there is an entry before this one, remove iterator from the list. */
		if(prev)
		{
			prev->next = iterator->next;
		}
		/* else: this was the first entry in the queue. */
		else
		{
			not_ready_queue = iterator->next;
		}

		/* Add the thread to the correct queue. */
		tm_add_thread_to_queue(iterator);
		return 0;
	}

	/* Failure! */
	return 1;
}

/** Returns the thread structure of the current running thread. 					**/
thread_t *tm_thread_get_current_thread(void)
{
	/** Hide the ugly system-functions. */
	return system_c::get_current_thread();;
}

/** Returns the thread-id of the current running thread. 						**/
uint64_t tm_thread_get_current_thread_thid(void)
{
	/* This function might be excecuted twice at the same time->registers! */
	register thread_t *curr asm("r12") = system_c::get_current_thread();;

	if(!curr)
	{
		return 0;
	}

	/* Return the thid. */
	return curr->thid;
}

/** Thread exist routine. 										**/
void thread_exit(void)
{
	/* Read return value from rax. */
	uint64_t val;
	asm volatile ("movq %%rax, %0;":"=r"(val));

	/* On which thread was this cpu running? */
	register cpu_c *cpu =  system_c::get_current_cpu();

	/* Which thread just exited? */
	register thread_t *thread = system_c::get_current_thread();
	printf("%s with thid %d running on cpu %d existed with value: %x.\n",thread->name, thread->thid, cpu->get_id(), val);

	/* Kill the thread. */
	tm_kill_current_thread();
}

void tm_kill_current_thread()
{
	register scheduler_c *scheduler = system_c::get_current_scheduler();
	scheduler->stop_current_thread();
}


/** Creates an idle thread, one idle thread is required per logical cpu. See Scheduler Constructor.	**/
thread_t *tm_thread_create_idle_thread(void)
{
	/* Create and initialise an entry thread structure. */
	thread_t *idle 	= (thread_t*)malloc(sizeof(thread_t));
	idle->next	= 0;
	idle->thid	= __sync_add_and_fetch(&tm_current_thid, 1);
	idle->name	= "Idle";
	idle->flags	= 0x0;
	idle->quantum	= 10;
	idle->priority	= 0x3;
	idle->parent	= 0;

	/* Setup a user/kernel stack (kthread so user stack = kernel stack) */
	uint64_t *stack = (uint64_t*)((uint64_t)malloc(0x8000)+0x1000);

	/* Prepare the thread stack. Set the intial register values. */
	*--stack	= (uint64_t)&thread_exit;
	*--stack 	= (uint64_t)0x10;		// Stack segment selector
	uint64_t usrrsp = (uint64_t)stack + 8;
	*--stack 	= (uint64_t)((uint64_t)usrrsp);	// Pointer to stack
	*--stack 	= (uint64_t)0x200; 		// Interrupts enabled
	*--stack 	= (uint64_t)0x08; 		// Code segment selector
	*--stack 	= (uint64_t)&tm_idle_thread_fn; // RIP
	stack 		-= 2;				// RAX, RBX
	*--stack	= 0; 				// TSPR
	stack 		-= 10;				// RBX, RCX RDX, ...
	*--stack 	= 0;				// rbp
	*--stack 	= (uint64_t)0;			// rdi
	*--stack 	= (uint64_t)0;			// rsi
	*--stack 	= (uint64_t)0x10;		// Setup data segment
	*--stack 	= (uint64_t)0x10000;		// Setup PLM4T for this thread
	idle->rsp	= (uint64_t)stack;		// pointer to the stack

	/* Return pointer to the idle thread structure. */
	return idle;
}

/** The function that will be executed if the current logical cpu is idling. 				**/
void tm_idle_thread_fn(void)
{
	/* TODO: Maybe power down cpu? */
	while(1)
	{
		/* Halt so CPU can enter sleep state. */
		asm volatile("hlt");
	}
}

/** Gets called by the timer routine to swap the current running thread with a new one from the queue. **/
uint64_t tm_schedule(uint64_t rsp)
{
	/* Select the correct scheduler and schedule. */
	return system_c::get_current_scheduler()->schedule(rsp);
}

/* Adds a thread to the cpu with the lowest load. Returns ID of the used scheduler.			**/
uint32_t tm_add_thread_to_queue(thread_t *thread)
{
	register uint32_t priority = lapic_read(0x80);
	lapic_write(0x80, 0xFF);

	/* This code might be excecuted twice at the same time, so registers. */
	register scheduler_c *iterator = system_c::get_lowest_load_scheduler();

	iterator->add_thread(thread);
	iterator->increase_load();

	lapic_write(0x80, priority);

	/* Return the ID of the scheduler we used. */
	return 0;
}

/** Puts the current running thread to sleep for a given amount of milli seconds. 			**/
void tm_thread_sleep(uint64_t millis)
{
	/* Critical code: Dissable Interrupts. */
	asm volatile("cli");
	uint32_t tpr = lapic_read(0x80);
	lapic_write(0x80, 0xFF);

	system_c::get_current_scheduler()->remove_thread(system_c::get_current_thread());

	/* Acquire the spinlock. */
	acquireLock(&sleep_lock);

	/* This code might be excecuted twice at the same time, so use registers. */
	register scheduler_c *sched = system_c::get_current_scheduler();

	/* Is there a sleep queue? */
	if(sched_sleep_queue)
	{
		/* Setup up some temporary pointers for iterating through the list. */
		register thread_t *iterator 	= (thread_t *)sched_sleep_queue;
		register thread_t *prev 	= 0;

		/* Iterate */
		while(iterator->next)
		{
			if(millis < iterator->delta_time)
			{
				/* Stick the entry before the next one. */
				sched->get_current_thread()->delta_time = millis;
				/* Is this the begining of the queue? */
				if(prev)
				{
					/* Nope so adjust pointer of previous entry */
					prev->next = sched->get_current_thread();
				}
				else
				{
					/* Yup, so change sleep queue pointer. */
					sched_sleep_queue = sched->get_current_thread();
				}
				sched->get_current_thread()->next = iterator;

				/* Adjust all delta times after this entry. */
				while(iterator)
				{
					iterator->delta_time 	-= millis;
					iterator 		= iterator->next;
				}

				/* Set flag that this thread should be stopped. */
				sched->get_current_thread()->flags |= THREAD_FLAG_STOPPED;

				/* Reenable interrupts, release the lock and yield. */
				releaseLock(&sleep_lock);
				lapic_write(0x80, tpr);
				asm volatile("sti");
				asm volatile("int $33");
				return;
			}

			/* Adjust for delta, and iterate. */
			millis 		-= iterator->delta_time;
			prev 		= iterator;
			iterator 	= iterator->next;
		}

		/* Should we place this thread right before the last entry? */
		if(millis < iterator->delta_time)
		{

			sched->get_current_thread()->delta_time = millis;

			/* Is there a thread before this one? */
			if(prev)
			{
				/* Yup, adjust its pointer to the next thread. */
				prev->next = sched->get_current_thread();
			}
			else
			{
				/* Nope, this is the new first entry of the list! */
				sched_sleep_queue = sched->get_current_thread();
			}

			/* Set flag that this thread should be stopped and set pointer to next entry */
			sched->get_current_thread()->flags 	|= THREAD_FLAG_STOPPED;

			/*Squeeze the thread in and adjust delta time of the next thread */
			sched->get_current_thread()->next 	= iterator;
			iterator->delta_time 			-= millis;

			/* Reenable interrupts, release the lock and yield. */
			releaseLock(&sleep_lock);
			lapic_write(0x80, tpr);
			asm volatile("sti");
			asm volatile("int $33");
			return;
		}
		if(millis >= iterator->delta_time)
		{
			/* Set flag that this thread should be stopped. */
			sched->get_current_thread()->flags	|= THREAD_FLAG_STOPPED;
			sched->get_current_thread()->delta_time = millis - iterator->delta_time;

			/*Set the pointer to our thread, and the the next one. */
			iterator->next 				= sched->get_current_thread();
			sched->get_current_thread()->next 	= 0;


			/* Reenable interrupts, release the lock and yield. */
			releaseLock(&sleep_lock);
			asm volatile("sti");
			lapic_write(0x80, tpr);
			asm volatile("int $33");
			return;
		}
	}
	else
	{
		/* Create a sleep queue and set the parameters. */
		sched_sleep_queue 			= (thread_t *)sched->get_current_thread();
		sched->get_current_thread()->delta_time = millis;
		sched->get_current_thread()->next 	= 0;

		/* Set flag that this thread should be stopped. */
		sched->get_current_thread()->flags	|= THREAD_FLAG_STOPPED;

		/* Reenable interrupts, release the lock and yield. */
		releaseLock(&sleep_lock);
		asm volatile("sti");
		lapic_write(0x80, tpr);
		asm volatile("int $33");
		return;
	}
}
