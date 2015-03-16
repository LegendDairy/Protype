/* Pro-Type Kernel v1.3		*/
/* Physical Memory v2.0		*/
/* By LegendMythe		*/

#ifndef PMM_H
#define PMM_H

#include <common.h>

typedef struct
{
	uint64_t base;
	uint64_t lenght;
	uint32_t type;
	uint32_t acpi;
} map_entry_t;

/** Initialises the Physical Memory Manager.	**/
/** Param1: The end of the kernel.		**/
/** Param2: The total memory size.		**/
/** Hybrid model: Bitmap + Stack.		**/
void setup_pmm(u32int start, u32int total);

/** Pushes the page and clears the bit.		**/
/** Arg1: the physical address of the page.	**/
void pmm_free_page(u32int addr);

/** Pops the page and sets the bit.		**/
/** Returns the physical address of the page.	**/
u32int pmm_alloc_page(void);

#endif
