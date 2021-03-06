/* Pro-Type Kernel v0.2		*/
/* Physical Memory v0.3		*/
/* By LegendDairy		*/

#include <pmm.h>
#include <mutex.h>

uint64_t *pmm_bmap				= 0;
uint64_t *pmm_stack				= 0;
uint64_t pmm_top				= 0;
uint64_t pmm_nframes				= 0;
mutex_t  pmm_lock;
map_entry_t *pre_pmm_heap			= 0;
uint64_t pre_pmm_entries			= 0;

/* Current TODO: 				*/
/* -mcmodel=large, maybe alternatives?		*/
/* Better testing maybe. 			*/

/** Initialises the Physical Memory Manager.            	**/
void setup_pmm(ipl_info_t *info)
{
	mutex_setup(&pmm_lock);

	/* Page allign the end of kernel */
	end = (uint64_t)&end;

	if (end % 0x1000)
	{
		end += (0x1000 - (end % 0x1000));
	}


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

	i 	= 0;
	tmp 	= info->mmap;

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
					/* Split the chunk */
					tmp->size -= end;
					tmp->base = end;
				}
				else
				{
					/* Erase/Allocate entry */
					tmp->base = 0;
					tmp->size = 0;
				}
			}
		}
		tmp++;
	}

	/* Set up Pre-PMM Heap */
	pre_pmm_heap	= info->mmap;
	pre_pmm_entries = info->mmap_entries;
	pmm_nframes 	= (info->mem_sz / 0x1000);

	/* Calculate size of memory needed for the pmm */
	uint64_t sz = (pmm_nframes / 8) + (pmm_nframes * 8);

	if (sz%0x1000)
	{
		sz += (0x1000 - sz % 0x1000);
	}

	/* Allocate memory for PMM*/
	tmp	= info->mmap;
	i 	= 0;

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

	/* Map the memory for PMM */
	i 	= 0;
	pmm_top = (uint64_t)pmm_stack;

	while(i<(uint64_t)pmm_stack)
	{
		pre_vmm_map_frame(i, i, 0x3);
		i += 0x1000;

	}

	memset((uint8_t*)pmm_bmap, 0xFF, (pmm_nframes/8));
	memset((uint8_t*)(pmm_stack - (pmm_nframes)), 0, pmm_nframes * 8);

	/* Setup PMM Bitmap and stack */
	i	= info->mmap_entries;
	tmp	= info->mmap;

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

	/* Page allign top of pmm */
	if (pmm_top % 0x1000)
	{
		pmm_top += (0x1000 - pmm_top % 0x1000);
	}
}

/** Physical page allocator to be used before pmm is setup. 	**/
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

/** Pushes the page and clears the bit.                 	**/
void pmm_free_page(uint64_t address)
{
	mutex_lock(&pmm_lock);
	if(pmm_test_bmap(address))
	{

		pmm_bclear(address);
		pmm_stack--;
		*pmm_stack = address;
	}
	mutex_unlock(&pmm_lock);
}

/** Pops a page from the stack and sets the bit.        	**/
uint64_t pmm_alloc_page(void)
{
	mutex_lock(&pmm_lock);
	if ((uint64_t)pmm_stack < pmm_top)
	{
		uint64_t address = *pmm_stack;
		pmm_stack++;
		pmm_bset(address);
		mutex_unlock(&pmm_lock);
		return address;
	}
	mutex_unlock(&pmm_lock);
	for(;;)
	return 0;
}

/** Test if a bit in the bitmap is set. 			**/
uint64_t pmm_test_bmap(uint64_t address)
{
	address = address / 0x1000;
	uint64_t ind = address / 64;
	uint64_t off = (address % 64);
	return (pmm_bmap[ind] & (1 << off));
}

/** Sets a bit in the bitmap.		 			**/
void pmm_bset(uint64_t address)
{
	address = address / 0x1000;
	uint64_t ind = (address / 64);
	uint64_t off = (address % 64);
	pmm_bmap[ind] |= (1 << off);
}

/** Clears a bit in the bitmap.		 			**/
void pmm_bclear(uint64_t address)
{
	address = address / 0x1000;
	uint64_t ind = (address / 64);
	uint64_t off = (address % 64);
	pmm_bmap[ind] &= (uint64_t)(~(1 << off));
}
