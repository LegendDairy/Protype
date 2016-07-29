/* Pro-Type Kernel v1.3		*/
/* Threads v0.3			*/
/* By LegendMythe		*/

#include <thread.h>

uint64_t tm_current_thid 	= 1;
extern thread_list_t *current_thread;

void tm_create_thread(int (*fn)(void*), uint64_t PLM4T, uint8_t priority, uint64_t quantum, const char *name, uint32_t flags, uint64_t *stack, uint8_t ds, uint8_t cs, uint8_t ss)
{
	/* Create and initialise an entry thread structure. */
	thread_t *entry 	= malloc(sizeof(thread_t));
	entry->next		= 0;
	entry->thid		= tm_current_thid++;
	entry->name		= name;
	thread->flags		= flags;
	thread->quantum		= quantum;
	thread->priority	= priority;
	thread->parent_thid	= current_thread->thid;

	/* Prepare the thread stack. Set the intial register values. */
	//*--stack = (uint64_t)arg;
	//*--stack = (uint64_t)&thread_exit;

	*--stack = (uint64_t)fn;
	*--stack = (uint64_t)cs;
	*--stack = (uint64_t)0x200; // Interrupts enabled.
	*--stack = (uint64_t)(rsp-1);
	*--stack = (uint64_t)ss;

	//memsetq(stack, 0, 12);			// GPR are 0
	stack 	-= 15;					//make room for the GPR on the stack.
	*--stack = (uint64_t)ds;
	*--stack = (uint64_t)PLM4T;

	/* Add to not ready queue. */

}
