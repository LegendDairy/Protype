/* Pro-Type Kernel	v1.3	*/
/* Virtual Memory	v0.3	*/
/* By LegendMythe		*/

#include <vmm.h>

/*
S: Single responsibility: OBJECT should have 1 primary res, reason to exist
O open/closed principle
L
I
D
*/
/*
Every pml4 entry addresses 0x8000000000 bytes of virtual memroy */

#define PML4T_VADDRESS					0xFFFFFFFFFFFFF000
#define PDPT_VADDRESS					0xFFFFFFFFFFE00000
#define PD_VADDRESS					0xFFFFFFFFC0000000
#define PT_VADDRESS			                0xFFFFFF8000000000
							0x0000008000000000
#define PAGE_SIZE                                       0x1000

#define PLM4T_INDEX(a)					((uint64_t)a)/((uint64_t)512*1024*1024*1024)
#define PDPT_INDEX(a)					((uint64_t)a)/((uint64_t)1024*1024*1024)
#define PD_INDEX(a)			                ((uint64_t)a)/((uint64_t)2*1024*1024)
#define PT_INDEX(a)			                ((uint64_t)a)/((uint64_t)PAGE_SIZE)

extern "C" void flush_tlb(void);

class vmm_directory
{
private:
	uint64_t *pml4t;
	uint64_t *pdpt;
	uint64_t *directories;
	uint64_t *tables;
	uint64_t pml4t_pa;
	mutex_t lock;

public:
	/* Constructor */
	vmm_directory(void);
	/* Before using the object, always prepare to use the object. */
	void prepare(void);
	void map_frame(uint64_t va, uint64_t pa, uint16_t flags);
	void unmap_frame(uint64_t va);
	bool test_mapping(uint64_t va);
	uint64_t get_mapping(uint64_t va);
	uint64_t get_pml4t_pa(void);
};

vmm_directory::vmm_directory(void)
{
	pml4t_pa	= pmm_alloc_page();
	pml4t 		= (uint64_t *)PML4T_VADDRESS;
	pdpt 		= (uint64_t *)PDPT_VADDRESS;
	directories 	= (uint64_t *)PD_VADDRESS;
	tables 		= (uint64_t *)PT_VADDRESS;
	mutex_unlock(&lock);
}

void vmm_directory::map_frame(uint64_t va, uint64_t pa, uint16_t flags)
{
	mutex_lock(&lock);

	if (!(pml4t[PML4T_INDEX(va)] & 0x1))
	{
		pml4t[PML4T_INDEX(va)] = pmm_alloc_page() | 0x3;
		memset((uint8_t *)(DIR_PTRS_VADDRESS + PML4T_INDEX(va) * 0x1000), 0x00, 0x1000);
	}

	if (!(pdpt[PDPT_INDEX(va)] & 0x1))
	{
		pdpt[PDPT_INDEX(va)] = pmm_alloc_page() | 0x3;
		memset((uint8_t *)(DIRS_VADDRESS + PDPT_INDEX(va) * 0x1000), 0x00, 0x1000);
	}

	if (!directories[PD_INDEX(va)] & 0x1))
	{
		directories[PD_INDEX(va)] = pmm_alloc_page() | 0x3;
		memset((uint8_t *)(TABLES_VADDRESS + PD_INDEX(va) * 0x1000), 0x00, 0x1000);
	}

	if ((tables[PT_INDEX(va)] & 0x1))
	{
		printf("[VMM] Error 0x1: Trying to remap a page!: %x\n", va);
		printf("[VMM] Kernel halted...\n");
		for (;;);
	}

	tables[PT_INDEX(va)] = (pa | flags);

	mutex_unlock(&lock);
}

/* Unmaps a given virtual address va. */
void vmm_directory::unmap_frame(uint64_t va)
{
	mutex_lock(&lock);
	if(test_mapping(va)
	{
		tables[PT_INDEX(va)] = 0;
		asm volatile("invlpg (%0)" ::"r" (va) : "memory");
		/* send_tlb_ipi(); */
	}
	mutex_unlock(&lock);
}

/* Returns the physical address of a virtual address va and can store this in pa. */
uint64_t vmm_directory::get_mapping(uint64_t va, uint64_t *pa)
{
	if ((pml4t[PML4T_INDEX(va)] & 0x1))
	{
		if ((pdpt[PDPT_INDEX(va)] & 0x1))
		{
			if ((directories[PD_INDEX(va)] & 0x1))
			{
				if(pa)
				{
					*pa = (tables[PT_INDEX(va)] & (!0xFFF));
				}
				return (tables[PT_INDEX(va)] & (!0xFFF));
			}
		}
	}
	return 0xFFF; // Error: va is not mapped!
}

uint64_t vmm_directory::test_mapping(uint64_t va)
{
	if ((pml4t[PML4T_INDEX(va)] & 0x1))
	{
		if ((pdpt[PDPT_INDEX(va)] & 0x1))
		{
			if ((directories[PD_INDEX(va)] & 0x1))
			{
				return (tables[PT_INDEX(va)] & (0x1));
			}
		}
	}
	return 0;
}

uint64_t vmm_directory::get_pml4t_pa(void)
{
	return pml4t_pa;
}

void vmm_directory::prepare()
{
	mutex_lock(&lock);
	pml4t[511] = pml4t | 0x3;
	asm volatile (
	        "mov   %%cr3, %%rax   \n\t"
	        "xor   %%rax, %%cr3   \n\t");
	mutex_unlock(&lock);
}
