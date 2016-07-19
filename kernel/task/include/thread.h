/* Pro-Type Kernel v1.3	*/
/* Thread Manager  v1.5 */
/* By LegendMythe       */

#ifndef THREAD_H
#define THREAD_H

#include <malloc.h>
#include <scheduler.h>
#include <stdint.h>
#include <vmm.h>
#include <heap.h>

#define THREAD_CHILD                    0x1
#define THREAD_READY                    0x0
#define THREAD_NREADY                   0x2
#define THREAD_SLEEP                    0x2
#define THREAD_PARENT                   0x4
#define THREAD_PROCESS                  0x4
#define THREAD_KERNEL                   0x8

#define THREAD_PRIORITY_HIGHEST		60
#define THREAD_PRIORITY_VERY_HIGH	50
#define THREAD_PRIORITY_HIGH		40
#define THREAD_PRIORITY_NORMAL		30
#define THREAD_PRIORITY_LOW             20
#define THREAD_PRIORITY_VERY_LOW	10
#define THREAD_PRIORITY_LOWEST		5

#define DS_USER                         0x23
#define DS_KERNEL			0x10
#define CS_USER				0x1B
#define CS_KERNEL			0x8
#define SS_USER				0x23
#define SS_KERNEL			0x10

#define KERNEL_STACK_SIZE               0x4000       // Use a 16kb kernel stack.
#define KERNEL_STACK_BASE               0xC0000000

extern thread_list_t *current_thread;

typedef struct thread_struct
{
  regs_t regs;	// Registers.
  u8int_t flags;																// Flags of the thread.													// Start of the Thread's heap.
  uint64_tint thid;																// Thread ID
  char *name;																// Name of the thread.
  uint64_t millis;															// Current relative wait value
  uint64_t priority;															// Priority level.
  uint64_t pr_lvl;															// Protection Level.
  uint64_t parent_thid;														// Thread ID of the parent thread.
  struct thread_list *childs;													// List of child threads.
} thread_t;

typedef struct thread_list
{
  struct thread_struct *thread;
  struct thread_list *next, *prev;
} thread_list_t;

typedef int(*fn_t)(void*);


#endif
