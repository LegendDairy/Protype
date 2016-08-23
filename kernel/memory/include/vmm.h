/* Pro-Type Kernel v0.2		*/
/* Virtual Memory v0.1		*/
/* By LegendDairy		*/

#ifndef VMM_H
#define VMM_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <pmm.h>
#include <mutex.h>

#define PLM4T_VADDRESS					0xFFFFFFFFFFFFF000
#define DIR_PTRS_VADDRESS				0xFFFFFFFFFFE00000
#define DIRS_VADDRESS					0xFFFFFFFFC0000000
#define TABLES_VADDRESS			                0xFFFFFF8000000000
#define PAGE_SIZE                                       0x1000

#define PLM4T_INDEX(a)					((uint64_t)a)/((uint64_t)512*1024*1024*1024)
#define PDPT_INDEX(a)					((uint64_t)a)/((uint64_t)1024*1024*1024)
#define PD_INDEX(a)			                ((uint64_t)a)/((uint64_t)2*1024*1024)
#define PT_INDEX(a)			                ((uint64_t)a)/((uint64_t)PAGE_SIZE)

#define PAGE_PRESENT					0x1
#define PAGE_WRITE				        0x2
#define PAGE_USER				        0x4
#define PAGE_WRITE_THROUGH			        0x8
#define PAGE_CACHE_DISSABLED		                0x10
#define PAGE_ACCESSED					0x20
#define PAGE_DIRTY					0x40
#define PAGE_GLOBAL					0x80

#define TABLE_PRESENT					0x1
#define TABLE_WRITE					0x2
#define TABLE_USER					0x4
#define TABLE_WRITE_THROUGH			        0x8
#define TABLE_CACHE_DISSABLED		                0x10
#define TABLE_ACCESSED					0x20
#define TABLE_LARGE_SZ					0x40
#define TABLE_IGNORED					0x80

/* Unlocks the vmm spinlock. */
void setup_vmm(void);
/* Maps a given frame (pa) to a given virtual address (va) to kernel PLM4T before the pmm is setup. */
void pre_vmm_map_frame(uint64_t va, uint64_t pa, uint64_t flags);
/* Maps a given frame (pa) to a given virtual address (va) to kernel PLM4T once the pmm is setup. */
void vmm_map_frame(uint64_t va, uint64_t pa, uint64_t flags);
/* Unmaps a given virtual address (va). */
void vmm_unmap_frame(uint64_t va);
/* Returns the physical address of a virtual address va and can store this in pa. */
uint64_t vmm_get_mapping(uint64_t va, uint64_t *pa);
/* Tests if a given page is mapped, returns 1, if va is mapped.*/
uint64_t vmm_test_mapping(uint64_t va);


/* TODO */
//void vmm_unmap_region(uint64_t start, uint64_t end);
//void vmm_map_large_frame(uint64_t va, uint64_t pa, uint64_t flags);

#endif
