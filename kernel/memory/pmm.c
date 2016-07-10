/* Pro-Type Kernel v1.3		*/
/* Physical Memory v2.0		*/
/* By LegendMythe		*/

#include <pmm.h>

uint64_t *pmm_bmap				= 0;
uint64_t *pmm_stack				= 0;

uint64_t pmm_top				= 0;
uint64_t pmm_nframes				= 0;
mutex_t  pmm_lock;

map_entry_t *pre_pmm_heap			= 0;
uint64_t pre_pmm_entries			= 0;

/* TODO MUTEX the shit out of alloc/free 	*/

/* Current BUGS/TODO: 				*/
/* -mcmodel=large, maybe alternatives?		*/
/* Bitmap just doesn't work wtf... 		*/

void setup_pmm(ipl_info_t *info)
{
	pmm_lock.lock = 0;

	/* Page allign the end of kernel */
	end = (uint64_t)&end;

	if (end % 0x1000)
	{
		end += (0x1000 - (end % 0x1000));
	}
	printf("[PMM] Kernel end page alligned at %x \n", end);

	/* Parse map & allocate the kernel memory */
	uint64_t i = 0;
	map_entry_t *tmp = info->mmap;
	uint64_t mem_size = 0;

	for (i=0; i < info->mmap_entries; i++)
	{
		if (tmp->type == 1)
		{
		if(mem_size < (tmp->base + tmp->size)) 
		{
			mem_size = (uint64_t)((tmp->base) + (tmp->size));
		}
		}
		tmp++;
	}
	printf("Memory size: %x", mem_size);
	i = 0;
	tmp = info->mmap;

	for (i=0; i < info->mmap_entries; i++)
	{
		if (tmp->type == 1)
		{
			/* Make sure all entries are page alligned */
			if (tmp->base % 0x1000)
			{
				if (tmp->size < 0x1000)
				{
					tmp->size = 0;
					tmp->base = 0;
				}
				else
				{
					uint64_t diff = (tmp->base % 0x1000);
					tmp->size -= diff;
					tmp->base += (0x1000 - diff);


				}
			}
			/* Make sure all entries are an integer number of pages in size */
			if (!tmp->size % 0x1000)
			{
				if (tmp->size < 0x1000)
				{
					tmp->size = 0;
					tmp->base = 0;
				}
				else
				{
					tmp->size -= (tmp->size % 0x1000);
				}
			}
			/* Allocate current Kernel Memory*/
			if (tmp->base < end)
			{
				if (tmp->size >= end)
				{
					tmp->size -= end;								// Split the chunk
					tmp->base = end;
				}
				else
				{
					tmp->base = 0;									// Erase/Allocate entry
					tmp->size = 0;
				}
			}
		}
		tmp++;
	}

	printf("[PMM] Memory map at %x, tmp at %x, estimated mmapsize: %x\n", info->mmap, tmp, info->mmap_entries*sizeof(map_entry_t));

	/* Set up Pre-PMM Heap */
	pre_pmm_heap	= info->mmap;
	pre_pmm_entries = info->mmap_entries;
	pmm_nframes = (info->mem_sz / 0x1000);
	printf("[PMM] nframes: %d\n",pmm_nframes);
	
	/* Calculate size of memory needed for the pmm */
	uint64_t sz = (pmm_nframes / 8) + (pmm_nframes * 8);

	if (sz%0x1000)
	{
		sz += (0x1000 - sz % 0x1000);
	}

	printf("[PMM] Estimated PMM page alligned memory size: %x\n",sz);

	/* Allocate memory for PMM*/
	tmp	= info->mmap;
	i = 0;

	for (i=0; i < info->mmap_entries; i++)
	{
		if (!pmm_bmap)
		{
			if (tmp->type == 1)
			{
				if (tmp->size >= sz)
				{
					pmm_bmap = (uint64_t*)(tmp->base);
					pmm_stack = (uint64_t*)(tmp->base + sz);
					tmp->size -= sz;
					tmp->base += sz;
				}
			}
		}
		tmp++;
	}

	/* Map the memory for PMM*/
	i = 0;
	pmm_top = (uint64_t)pmm_stack;

	printf("[PMM] PMM Bitmap at: %x, PMM stack at: %x\n", pmm_bmap, (uint64_t)pmm_stack );
	printf("[PMM] PMM mapping reserved memory...\n", ((uint64_t)pmm_bmap));

	while(i<(uint64_t)pmm_stack)
	{
		pre_vmm_map_frame(i, i, 0x3);
		i += 0x1000;

	}
	printf("[PMM] PMM erasing reserved memory...\n", ((uint64_t)pmm_bmap));
	memset((uint8_t*)pmm_bmap, 0xFF, (pmm_nframes/8));
	memset((uint8_t*)(pmm_stack - (pmm_nframes)), 0, pmm_nframes * 8);

	/* Setup PMM Bitmap and stack */
	i	= info->mmap_entries;
	tmp	= info->mmap;
	printf("[PMM] Freeing pages...\n", ((uint64_t)pmm_bmap));
	while (i)
	{
		if (tmp->type == 1)
		{
			while (tmp->size)
			{
				pmm_free_page(tmp->base);
				tmp->base += 0x1000;
				tmp->size -= 0x1000;

			}
		}
		tmp++;
		i--;
	}

	if (pmm_top % 0x1000)
	{
		pmm_top += (0x1000 - pmm_top % 0x1000);
	}

	/* Unmap remaining pages */
	i = 0x400000;
	while (i > pmm_top)
	{
		//vmm_unmap_frame(i);
		i -= 0x1000;
	}
}

uint64_t pre_pmm_allocate_frame(void)
{
	uint64_t i = 0;
	map_entry_t *tmp = pre_pmm_heap;
	uint64_t frame = 0;

	for (i=0; i < pre_pmm_entries; i++)
	{
		if (tmp->type == 1)
		{
			if (tmp->size >= 0x1000)
			{
				frame = tmp->base;
				tmp->size -= 0x1000;
				tmp->base += 0x1000;
				return frame;
			}
		}
		tmp++;
	}

	if (!frame)
	{
		printf("[PMM]: Error Finding memory for intitialisation!\n");
		printf("[Kernel]: Halted...\n");
	}
	return 0;
}

void pmm_free_page(uint64_t address)
{
	if(pmm_test_bmap(address))
	{
		pmm_bclear(address);
		pmm_stack--;
		*pmm_stack = address;
		
	}
}

uint64_t pmm_alloc_page(void)
{
	if ((uint64_t)pmm_stack < pmm_top)
	{
		uint64_t address = *pmm_stack;
		pmm_stack++;
		pmm_bset(address);
		return address;
	}
	for(;;)
	return 0;
}

uint64_t pmm_test_bmap(uint64_t address)
{
	address = address / 0x1000;
	uint64_t ind = address / 64;
	uint64_t off = (address % 64);
	return (pmm_bmap[ind] & (1 << off));
}

void pmm_bset(uint64_t address)
{
	address = address / 0x1000;
	uint64_t ind = (address / 64);
	uint64_t off = (address % 64);
	pmm_bmap[ind] |= (1 << off);
}

void pmm_bclear(uint64_t address)
{
	address = address / 0x1000;
	uint64_t ind = (address / 64);
	uint64_t off = (address % 64);
	pmm_bmap[ind] &= (uint64_t)(~(1 << off));
}
        
