/* Pro-Type Kernel v1.3       */
/* Dynamic Memory Manager 1.1 */
/* By LegendMythe             */

#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <vmm.h>
#include <pmm.h>


/* Hiding some ugly numbers. */
<<<<<<< HEAD
#define HEAP_START	0x90000000
=======
#define HEAP_START	0x80000000
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
#define MAGIC		0xABABABAB

/* Memorybock header. */
typedef struct header
{
<<<<<<< HEAD
  uint64_t magic0;
  struct header *next;
  struct header *prev;
  uint8_t allocated;
  uint64_t size;
  uint64_t magic1;
=======
  u32int magic0;
  struct header *next;
  struct header *prev;
  u8int allocated;
  u32int size;
  u32int magic1;
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
} __attribute__((packed)) header_t;

/** Dynamic allocater, chrashed if you overwrite your chunk! **/
/** arg1: Size of the piece you wish to allocate.			 **/
/** Return:  pointer to a virtual address					 **/
<<<<<<< HEAD
void *malloc(uint64_t sz);
=======
void *malloc(u32int sz);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422

/** Frees an allocated address.			**/
/** Arg1: Virtual address of a block.	**/
void free (void *p);

#endif
