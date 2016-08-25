/* Pro-Type Kernel v0.2		*/
/* Thread Manager v0.1		*/
/* By LegendDairy		*/

#ifndef THREAD_H
#define THREAD_H

#include <common.h>

/** Typedefinition for the thread structure.						**/
typedef struct thread_t thread_t;
/** Type definition for a pointer to a function.					**/
typedef int(*fn_t)(uint64_t argn, char **argv);

/** Creates a new thread and adds it to the not-ready queue. Returns Thread id. 			**/
uint64_t tm_thread_create(fn_t fn, uint64_t argn, char *argv[], uint64_t PLM4T, uint8_t priority, uint64_t quantum, const char *name, uint32_t flags, uint64_t *stack, uint8_t ds, uint8_t cs, uint8_t ss);
/** Creates an idle thread, one idle thread is required per logical cpu. See Scheduler Constructor.	**/
thread_t *tm_thread_create_idle_thread(void);
/** Returns current thread structure. 									**/
thread_t *tm_thread_get_current_thread(void);
/** Returns current thread id.										**/
uint64_t tm_thread_get_current_thread_thid(void);
/** Starts a thread in the not ready queue. 								**/
uint64_t tm_thread_start(uint64_t thid);
/** Adds the current running thread to the end of the appropriate list. Return CPU id.			**/
uint32_t tm_add_thread_to_queue(thread_t *thread);
/** Stops excecution of current running thread. 							**/
void tm_kill_current_thread(void);
/** Puts the current running thread to sleep for a given amount of milli seconds. 			**/
void tm_thread_sleep(uint64_t millis);

typedef struct thread_t
{
  thread_t *next;	// Pointer to the next thread (linked list)
  uint64_t thid;	// Threads ID number
  const char *name;	// Thread name (for debugging)
  uint32_t flags;	// Thread flags (see below)
  uint64_t rsp;		// Thread stack pointer
  uint64_t kstack;	// TSS kernel stack (TODO)
  uint64_t quantum;	// Thread's quantum (=alive time)
  uint32_t delta_time;	// Sleep time (delta-linked-list)
  uint8_t priority;	// Thread's priority level (1, 2 or 3)
  uint64_t parent;	// ID of the thread that created this one
} thread_t;

#define THREAD_FLAG_READY          	0x01
#define THREAD_FLAG_KERNEL              0x02
#define THREAD_FLAG_STOPPED 		0x10
#define THREAD_PRIORITY_HIGHEST		0x01
#define THREAD_PRIORITY_NORMAL		0x02
#define THREAD_PRIORITY_LOWEST          0x03
#define DS_USER                         0x23
#define DS_KERNEL			0x10
#define CS_USER				0x1B
#define CS_KERNEL			0x8
#define SS_USER				0x23
#define SS_KERNEL			0x10
#define KERNEL_STACK_SIZE               0x1000
#define KERNEL_STACK_BASE               0xC0000000

#endif
