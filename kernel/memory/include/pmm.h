/* Pro-Type Kernel v1.3		*/
/* Physical Memory v0.2		*/
/* By LegendMythe		*/

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdio.h>
#include <vmm.h>
#include <string.h>
#include <ipl.h>
#include <mutex.h>


extern uint64_t end;

/** Initialises the Physical Memory Manager.	**/
/** Param1: Pointer to IPL Info Structure		**/
/** Hybrid model: Bitmap + Stack.				**/
void setup_pmm(ipl_info_t *info);

/** Pushes the page and clears the bit.		**/
/** Arg1: the physical address of the page.	**/
void pmm_free_page(uint64_t address);

/** Pops the page and sets the bit.		**/
/** Returns the physical address of the page.	**/
uint64_t pmm_alloc_page(void);

uint64_t pmm_test_bmap(uint64_t address);
void pmm_bclear(uint64_t address);
void pmm_bset(uint64_t address);

uint64_t pre_pmm_allocate_frame(void);
void pmm_errase_frame(uint64_t frame);

#endif
