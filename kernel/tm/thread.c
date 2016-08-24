/* Pro-Type Kernel v0.2		*/
/* Thread Manager v0.1		*/
/* By LegendDairy		*/

#include <thread.h>
#include <scheduler.hpp>
#include <system.hpp>
#include <acpi.h>
#include <apic.h>
#include <heap.h>

uint64_t tm_current_thid 	= 0;
thread_t *not_ready_queue 	= 0;

static void thread_exit(void);
static void tm_idle_thread_fn(void);

/** Creates a new thread and adds it to the not-ready queue. Returns Thread id. 	**/
uint64_t tm_thread_create(fn_t fn, uint64_t argn, char *argv[], uint64_t PLM4T, uint8_t priority, uint64_t quantum, const char *name, uint32_t flags, uint64_t *stack, uint8_t ds, uint8_t cs, uint8_t ss)
{

	/* Create and initialise an entry thread structure. */
	thread_t *entry 	= (thread_t*)malloc(sizeof(thread_t));
	entry->next		= 0;
	entry->thid		= __sync_add_and_fetch(&tm_current_thid, 1);
	entry->name		= name;
	entry->flags		= flags;
	entry->quantum		= quantum;
	entry->priority		= priority;
	entry->parent_thid	= tm_thread_get_current_thread_thid();

	/* Prepare the thread stack. Set the intial register values. */
	*--stack		= (uint64_t)&thread_exit;
	*--stack 		= (uint64_t)ss;					// Stack segment selector
	uint64_t usrrsp 	= (uint64_t)stack + 8;
	*--stack 		= (uint64_t)((uint64_t)usrrsp);			// Pointer to stack
	*--stack 		= (uint64_t)0x200; 				// Interrupts enabled
	*--stack 		= (uint64_t)cs; 				// Code segment selector
	*--stack 		= (uint64_t)fn; 				// RIP

	//memsetq(stack, 0, 12);						// Set gprs to 0
	stack 			-= 2;
	*--stack		= 0; // TSPR
	stack 			-= 10;						// Make room for the GPR on the stack.
						// Make room for the GPR on the stack.
	*--stack 		= 0;						// rbp
	*--stack 		= (uint64_t)argn;				// rdi
	*--stack 		= (uint64_t)argv;				// rsi

	*--stack 		= (uint64_t)ds;					// Setup data segment
	*--stack 		= (uint64_t)PLM4T;				// Setup PLM4T for this thread
	entry->rsp		= (uint64_t)stack;				// pointer to the stack

	if(not_ready_queue)
	{
		thread_t *iterator = not_ready_queue;
		while(iterator->next)
		{
			iterator = iterator->next;
		}
		iterator->next = entry;
	}
	else
	{
		not_ready_queue = entry;
	}

	/* Add to not ready queue. */
	tm_thread_start(entry->thid);
	return entry->thid;
}

/** Searches the not ready queue for a thread with the given thid. 			**/
uint64_t tm_thread_start(uint64_t thid)
{
	thread_t *iterator 	= not_ready_queue;
	thread_t *prev 		= 0;

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
		tm_sched_add_to_queue(iterator);
		return 0;
	}

	/* Failure! */
	return 1;
}

/** Returns the thread structure of the current running thread. 			**/
thread_t *tm_thread_get_current_thread(void)
{
	return system_c::get_current_thread();;
}

/** Returns the thread-id of the current running thread. 				**/
uint64_t tm_thread_get_current_thread_thid(void)
{
	register thread_t *curr asm("r12") = system_c::get_current_thread();;

	if(!curr)
	{
		return 0;
	}

	return curr->thid;
}

/** Thread exist routine. 								**/
void thread_exit(void)
{
	uint64_t val;
	asm volatile ("movq %%rax, %0;":"=r"(val));
	register cpu_c *cpu =  system_c::get_current_cpu();
	register thread_t *thread = system_c::get_current_thread();
	printf("%s with thid %d running on cpu %d existed with value: %x.\n",thread->name, thread->thid, cpu->get_id(), val);
	tm_sched_kill_current_thread();
}

/** Creates an idle thread, one idle thread is required per logical cpu. 		**/
thread_t *tm_thread_create_idle_thread(void)
{
	/* Create and initialise an entry thread structure. */
	thread_t *idle 		= (thread_t*)malloc(sizeof(thread_t));
	idle->next		= 0;
	idle->thid		= __sync_add_and_fetch(&tm_current_thid, 1);
	idle->name		= "Idle";
	idle->flags		= 0x0;
	idle->quantum		= 10;
	idle->priority		= 0x3;
	idle->parent_thid	= 0;

	uint64_t *stack 	= (uint64_t*)((uint64_t)malloc(0x8000)+0x1000);

	/* Prepare the thread stack. Set the intial register values. */
	*--stack		= (uint64_t)&thread_exit;
	*--stack 		= (uint64_t)0x10;			// Stack segment selector
	uint64_t usrrsp 	= (uint64_t)stack + 8;
	*--stack 		= (uint64_t)((uint64_t)usrrsp);		// Pointer to stack
	*--stack 		= (uint64_t)0x200; 			// Interrupts enabled
	*--stack 		= (uint64_t)0x08; 			// Code segment selector
	*--stack 		= (uint64_t)&tm_idle_thread_fn; 	// RIP

	stack 			-= 2;
	*--stack		= 0; 			// TSPR
	stack 			-= 10;
	*--stack 		= 0;			// rbp
	*--stack 		= (uint64_t)0;		// rdi
	*--stack 		= (uint64_t)0;		// rsi

	*--stack 		= (uint64_t)0x10;	// Setup data segment
	*--stack 		= (uint64_t)0x10000;	// Setup PLM4T for this thread
	idle->rsp		= (uint64_t)stack;	// pointer to the stack

	return idle;
}

/** The function that will be executed if the current logical cpu is idling. 		**/
void tm_idle_thread_fn(void)
{
	while(1)
	{
		asm volatile("hlt");
	}
}
