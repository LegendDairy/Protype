/* Pro-Type Kernel v0.2		*/
/* Dynamic Memory Manager 0.1b  */
/* By LegendDairy		*/

#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <vmm.h>
#include <pmm.h>
 #include <stddef.h>

/* TODO                 */
/* -Data allignment.    */

/* Hiding some ugly numbers. */
#define HEAP_START	0x80000000
#define MAGIC		0xABABABABABABABAB

/* Memorybock header. */
typedef struct header
{
  uint64_t magic;
  struct header *next;
  struct header *prev;
  uint64_t allocated;
  uint64_t size;
} header_t;

/** Dynamic allocater for the kernel, chrashes if you overwrite your chunk!     **/
/** arg1: Size of the piece you wish to allocate.                               **/
/** Return:  pointer to a virtual address                                       **/
void *malloc(uint64_t sz);

/** Frees an allocated address.         **/
/** Arg1: Virtual address of a block.	**/
void free (void *p);

void *operator new(size_t size);

void *operator new[](size_t size);

void operator delete(void *p);

void operator delete[](void *p);

#endif
