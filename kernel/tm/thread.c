/* Pro-Type Kernel v1.3		*/
/* Threads v0.3			*/
/* By LegendMythe		*/

#include <thread.h>

uint64_t tm_current_thid 	= 1;
extern thread_t *current_thread;

uint64_t tm_thread_create(int (*fn)(void*), /*uint64_t argn, char *argv[],*/ uint64_t PLM4T, uint8_t priority, uint64_t quantum, const char *name, uint32_t flags, uint64_t *stack, uint8_t ds, uint8_t cs, uint8_t ss)
{
	/* Create and initialise an entry thread structure. */
	thread_t *entry 	= malloc(sizeof(thread_t));
	entry->next		= 0;
	entry->thid		= tm_current_thid++; // TODO: make atomic thid increase
	entry->name		= name;
	entry->flags		= flags;
	entry->quantum		= quantum;
	entry->priority		= priority;
	entry->parent_thid	= current_thread->thid;

	/* Prepare the thread stack. Set the intial register values. */
	//*--stack = (uint64_t)&thread_exit;
	//*--stack(rbp) = 0;
	//*--stack(rdi) = (uint64_t)argn;
	//*--stack(rsi) = (uint64_t)argv
	*--stack = (uint64_t)ss;		// Stack segment selector
	*--stack = (uint64_t)(stack+1);		// Pointer to stack
	*--stack = (uint64_t)0x200; 		// Interrupts enabled.
	*--stack = (uint64_t)cs; 		// Code segment selector
	*--stack = (uint64_t)fn; 		// RIP

	//memsetq(stack, 0, 12);			// GPR are 0
	stack 	-= 15;					//make room for the GPR on the stack.
	*--stack = (uint64_t)ds;
	*--stack = (uint64_t)PLM4T;
	entry->rsp		= stack;

	/* Add to not ready queue. */
	tm_sched_add_to_que(entry);

	return entry->thid;
}

void tm_start_thread(uint64_t thid)
{

}
