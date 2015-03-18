/* Pro-Type Kernel v1.3		*/
/* Physical Memory v2.0		*/
/* By LegendMythe		*/

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <string.h>

typedef struct
{
	uint64_t base;
	uint64_t size;
	uint32_t type;
	uint32_t acpi;
} map_entry_t;

extern uint64_t *end;

/** Initialises the Physical Memory Manager.	**/
/** Param1: The end of the kernel.		**/
/** Param2: The total memory size.		**/
/** Hybrid model: Bitmap + Stack.		**/
void setup_pmm(map_entry_t *mmap, uint64_t entries, uint64_t msize);

/** Pushes the page and clears the bit.		**/
/** Arg1: the physical address of the page.	**/
void pmm_free_page(uint64_t address);

/** Pops the page and sets the bit.		**/
/** Returns the physical address of the page.	**/
uint64_t pmm_alloc_page(void);

uint64_t pmm_test_bmap(uint64_t address);
void pmm_bclear(uint64_t address);
void pmm_bset(uint64_t address);

#endif
