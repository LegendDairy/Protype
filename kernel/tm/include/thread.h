/* Pro-Type Kernel v1.3	*/
/* Thread Manager  v1.5 */
/* By LegendMythe       */

#ifndef THREAD_H
#define THREAD_H

#include <heap.h>
#include <acpi.h>

#define THREAD_FLAG_READY          	0x01		// Else terminated
#define THREAD_FLAG_SLEEP               0x02
#define THREAD_FLAG_KERNEL              0x04
#define THREAD_FLAG_PROCESS		0x08
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

#define KERNEL_STACK_SIZE               0x4000
#define KERNEL_STACK_BASE               0xC0000000

typedef struct thread_t thread_t;

/* Structure with all the spinlocks for the scheduler. */
typedef struct
{
	uint32_t sched_ready_queue_high;
	uint32_t sched_ready_queue_med;
	uint32_t sched_ready_queue_low;
	uint32_t sched_notready_queue;
} sched_spinlock_t;

typedef struct thread_t
{
  thread_t *next;
  uint64_t thid;					// Threads ID number
  const char *name;					// Thread name (for debugging)
  uint32_t flags;					// Thread flags
  uint64_t rsp;						// Thread stack pointer
  uint64_t quantum;					// Thread's quantum (=alive time)
  uint32_t delta_time;
  uint8_t priority;					// Thread's priority level (1, 2 or 3)
  uint64_t parent_thid;					// ID of the thread that created this one
} thread_t;

typedef int(*fn_t)(uint64_t argn, char **argv);

uint64_t tm_thread_create(fn_t fn, uint64_t argn, char *argv[], uint64_t PLM4T, uint8_t priority, uint64_t quantum, const char *name, uint32_t flags, uint64_t *stack, uint8_t ds, uint8_t cs, uint8_t ss);

thread_t *tm_thread_create_idle_thread(void);
thread_t *tm_thread_get_current_thread(void);
uint64_t tm_thread_get_current_thread_thid(void);

#endif

// scheduling: H - M - H - M - H - L
