/* Pro-Type Kernel v0.2	*/
/* IPL Boot Info	*/
/* By LegendMythe	*/

#ifndef IPL_H
#define IPL_H

#include <stdint.h>

typedef struct
{
	uint64_t base;
	uint64_t size;
	uint32_t type;
	uint32_t acpi;
} __attribute__((packed)) map_entry_t;

typedef struct
{
	uint64_t magic;
	uint64_t mem_sz;
	uint64_t low_mem;
	uint64_t high_mem;
	map_entry_t *mmap;
	uint64_t mmap_entries;
	uint64_t driven;
	uint64_t drivetype;
	uint64_t kernel_PLM4T;
	uint64_t ramdisksize;
	uint64_t ramdiskptr;

} __attribute__((packed)) ipl_info_t;

#endif
