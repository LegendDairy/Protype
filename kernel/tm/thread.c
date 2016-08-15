/* Pro-Type Kernel v1.3		*/
/* Threads v0.3			*/
/* By LegendMythe		*/

#include <thread.h>

uint64_t tm_current_thid 	= 1;
extern topology_t *system_info;
void tm_sched_add_to_queue_synced(thread_t *thread);

extern sched_spinlock_t sched_lock;

static void thread_exit(void);

thread_t *tm_thread_get_current_thread(void)
{
	processor_t *curr = system_info_get_current_cpu();
	if(!curr)
	{
		return 0;
	}
	return curr->current_thread;
}

uint64_t tm_thread_get_current_thread_thid(void)
{
	thread_t *curr = tm_thread_get_current_thread();
	if(!curr)
	{
		return 0;
	}
	return curr->thid;
}

/* Creates a new thread. */
uint64_t tm_thread_create(fn_t fn, uint64_t argn, char *argv[], uint64_t PLM4T, uint8_t priority, uint64_t quantum, const char *name, uint32_t flags, uint64_t *stack, uint8_t ds, uint8_t cs, uint8_t ss)
{

	/* Create and initialise an entry thread structure. */
	thread_t *entry 	= (thread_t*)malloc(sizeof(thread_t));
	entry->next		= 0;
	entry->thid		= atomic_fetch_add(&tm_current_thid, 1);
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
	stack 			-= 12;						// Make room for the GPR on the stack.
	*--stack 		= 0;						// rbp
	*--stack 		= (uint64_t)argn;				// rdi
	*--stack 		= (uint64_t)argv;				// rsi

	*--stack 		= (uint64_t)ds;					// Setup data segment
	*--stack 		= (uint64_t)PLM4T;				// Setup PLM4T for this thread
	entry->rsp		= (uint64_t)stack;				// pointer to the stack

	asm("cli");
	/* Add to not ready queue. */
	tm_sched_add_to_queue_synced(entry);
	asm("sti");

	return entry->thid;
}

/* Finds a thread in the not_ready_queue and puts it in the correct queue. */
void tm_sched_kill_current_thread(void);
/* Thread exist routine. */
void thread_exit(void)
{
	uint64_t val;
	asm volatile ("movq %%rax, %0;":"=r"(val));
	processor_t *curr =  system_info_get_current_cpu();
	printf("%s with thid %d existed with value: %x.\n",curr->current_thread->name, curr->current_thread->thid, val);
	tm_sched_kill_current_thread();
}
