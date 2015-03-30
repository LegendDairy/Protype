/* Pro-Type Kernel	v1.3	*/
/* Virtual Memory	v0.1	*/
/* By LegendMythe		*/

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
#define TABLES_VADDRESS			              0xFFFFFF8000000000

#define PLM4T_INDEX(a)					((uint64_t)a)/((uint64_t)512*1024*1024*1024)
#define PDPT_INDEX(a)					((uint64_t)a)/((uint64_t)1024*1024*1024)
#define PD_INDEX(a)			                ((uint64_t)a)/((uint64_t)2*1024*1024)
#define PT_INDEX(a)			                ((uint64_t)a)/((uint64_t)4096)

#define PAGE_PRESENT					  0x1
#define PAGE_WRITE				          0x2
#define PAGE_USER				          0x4
#define PAGE_WRITE_THROUGH			          0x8
#define PAGE_CACHE_DISSABLED		                  0x10
#define PAGE_ACCESSED					  0x20
#define PAGE_DIRTY					  0x40
#define PAGE_GLOBAL					  0x80

#define TABLE_PRESENT					  0x1
#define TABLE_WRITE					  0x2
#define TABLE_USER					  0x4
#define TABLE_WRITE_THROUGH			          0x8
#define TABLE_CACHE_DISSABLED		                  0x10
#define TABLE_ACCESSED					  0x20
#define TABLE_LARGE_SZ					  0x40
#define TABLE_IGNORED					  0x80





void pre_vmm_map_frame(uint64_t va, uint64_t pa, uint64_t flags);
void vmm_map_frame(uint64_t va, uint64_t pa, uint64_t flags);
void vmm_unmap_frame(uint64_t va);
//void vmm_unmap_region(uint64_t start, uint64_t end);
//void vmm_map_large_frame(uint64_t va, uint64_t pa, uint64_t flags);

#endif
