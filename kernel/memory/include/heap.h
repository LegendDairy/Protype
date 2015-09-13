/* Pro-Type Kernel v1.3       */
/* Dynamic Memory Manager 1.1 */
/* By LegendMythe             */

#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <vmm.h>
#include <pmm.h>


/* Hiding some ugly numbers. */
#define HEAP_START	0x80000000
#define MAGIC		0xABABABAB

/* Memorybock header. */
typedef struct header
{

  uint64_t magic0;
  struct header *next;
  struct header *prev;
  uint8_t allocated;
  uint64_t size;
  uint64_t magic1;
} __attribute__((packed)) header_t;

/** Dynamic allocater, chrashed if you overwrite your chunk! **/
/** arg1: Size of the piece you wish to allocate.			 **/
/** Return:  pointer to a virtual address					 **/

void *malloc(uint64_t sz);

/** Frees an allocated address.			**/
/** Arg1: Virtual address of a block.	**/
void free (void *p);

#endif
