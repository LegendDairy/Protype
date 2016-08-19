/* Pro-Type Kernel	v1.3	*/
/* Virtual Memory	v0.2	*/
/* By LegendMythe		*/

#include <vmm.h>
#include <mutex.h>

volatile uint64_t *vmm_tables			= (uint64_t *)TABLES_VADDRESS;
volatile uint64_t *vmm_directories		= (uint64_t *)DIRS_VADDRESS;
volatile uint64_t *vmm_dir_ptrs			= (uint64_t *)DIR_PTRS_VADDRESS;
volatile uint64_t *vmm_plm4t			= (uint64_t *)PLM4T_VADDRESS;
mutex_t vmm_lock;
extern mutex_t heap_lock;

/* Current TODO: 				*/
/* -Send IPI when ivplg.			*/
/* -Better testing maybe. 			*/
/* -Test mutual exclusion (mutex) support.	*/
/* -Mapping to a given PLM4T. 			*/

void setup_vmm(void)
{
	mutex_setup(&vmm_lock);
	mutex_setup(&heap_lock);
}

/* Maps a given frame (pa) to a given virtual address (va) to kernel PLM4T before pmm is setup. */
void pre_vmm_map_frame(uint64_t va, uint64_t pa, uint64_t flags)
{
	/* Needs More Testing! */
	/* Check if a page directory pointer exists. */
	if (!(vmm_plm4t[PLM4T_INDEX(va)] & 0x1))
	{
		vmm_plm4t[PLM4T_INDEX(va)] = pre_pmm_allocate_frame() | 0x3;
		memset((uint8_t *)(DIR_PTRS_VADDRESS + PLM4T_INDEX(va) * 0x1000), 0x00, 0x1000);
	}
	/* Check if a pagedirectory exists. */
	if (!(vmm_dir_ptrs[PDPT_INDEX(va)] & 0x1))
	{
		vmm_dir_ptrs[PDPT_INDEX(va)] = pre_pmm_allocate_frame() | 0x3;
		memset((uint8_t *)(DIRS_VADDRESS + PDPT_INDEX(va) * 0x1000), 0x00, 0x1000);
	}
	/* Check if a pagetable exists. */
	if (!(vmm_directories[PD_INDEX(va)] & 0x1))
	{
		vmm_directories[PD_INDEX(va)] = pre_pmm_allocate_frame() | 0x3;
		memset((uint8_t *)(TABLES_VADDRESS + (PD_INDEX(va) * 0x1000)), 0x00, 0x1000);
	}
	/* Check if the page is already mapped */
	if (!(vmm_tables[PT_INDEX(va)] & 0x1))
	{
		/* Map the virtual address to the given physical address*/
		vmm_tables[PT_INDEX(va)] = (pa | flags);
	}
}

/* Maps a given frame (pa) to a given virtual address (va) to kernel PLM4T once the pmm is setup. */
void vmm_map_frame(uint64_t va, uint64_t pa, uint64_t flags)
{
	mutex_lock(&vmm_lock);

	if (!(vmm_plm4t[PLM4T_INDEX(va)] & 0x1))
	{
		vmm_plm4t[PLM4T_INDEX(va)] = pmm_alloc_page() | 0x3;
		memset((uint8_t *)(DIR_PTRS_VADDRESS + PLM4T_INDEX(va) * 0x1000), 0x00, 0x1000);
	}

	if (!(vmm_dir_ptrs[PDPT_INDEX(va)] & 0x1))
	{
		vmm_dir_ptrs[PDPT_INDEX(va)] = pmm_alloc_page() | 0x3;
		memset((uint8_t *)(DIRS_VADDRESS + PDPT_INDEX(va) * 0x1000), 0x00, 0x1000);
	}

	if (!(vmm_directories[PD_INDEX(va)] & 0x1))
	{
		vmm_directories[PD_INDEX(va)] = pmm_alloc_page() | 0x3;
		memset((uint8_t *)(TABLES_VADDRESS + PD_INDEX(va) * 0x1000), 0x00, 0x1000);
	}

	if ((vmm_tables[PT_INDEX(va)] & 0x1))
	{
		printf("[VMM] Error 0x1: Trying to remap a page!: %x\n", va);
		printf("[VMM] Kernel halted...\n");
		for (;;);
	}

	vmm_tables[PT_INDEX(va)] = (pa | flags);
	mutex_unlock(&vmm_lock);
}

/* Returns the physical address of a virtual address va and can store this in pa. */
uint64_t vmm_get_mapping(uint64_t va, uint64_t *pa)
{
	if ((vmm_plm4t[PLM4T_INDEX(va)] & 0x1))
	{
		if ((vmm_dir_ptrs[PDPT_INDEX(va)] & 0x1))
		{
			if ((vmm_directories[PD_INDEX(va)] & 0x1))
			{
				if(pa)
				{
					*pa = (vmm_tables[PT_INDEX(va)] & (!0xFFF));
				}
				return (vmm_tables[PT_INDEX(va)] & (!0xFFF));
			}
		}
	}
	return 0xFFF; // Error: va is not mapped!
}

uint64_t vmm_test_mapping(uint64_t va)
{
	if ((vmm_plm4t[PLM4T_INDEX(va)] & 0x1))
	{
		if ((vmm_dir_ptrs[PDPT_INDEX(va)] & 0x1))
		{
			if ((vmm_directories[PD_INDEX(va)] & 0x1))
			{
				return (vmm_tables[PT_INDEX(va)] & (0x1));
			}
		}
	}
	return 0;
}

/* Flushes the cpu TLB casche */
inline void vmm_flush_page(uint64_t vaddr)
{
	asm volatile("invlpg (%0)" ::"r" (vaddr) : "memory");
}

/* Unmaps a given virtual address va. */
void vmm_unmap_frame(uint64_t va)
{
	mutex_lock(&vmm_lock);
	vmm_tables[PT_INDEX(va)] = 0;
	vmm_flush_page(va);
	/* send_tlb_ipi(); */
	mutex_unlock(&vmm_lock);
}
