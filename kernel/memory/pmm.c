/* Pro-Type Kernel v1.3		*/
/* Physical Memory v2.0		*/
/* By LegendMythe		*/

#include <pmm.h>

uint64_t *pmm_bmap;
uint64_t *pmm_stack;
u64int_t pmm_nframes;

void setup_pmm(map_entry_t *mmap, uint64_t entries, uint64_t msize)
{
	
	pmm_bmap		= (uint64_t *)end;
	pmm_nframes		= msize / 0x1000
	uint64_t start	= (uint64_t)pmm_bmap + pmm_nframes/8;
	pmm_stack		= (uint64_t *)start + pmm_nframes*8;
	start			= pmm_stack + 8;

	memset(pmm_bmap, 0, pmm_nframes / 8);
	memset(pmm_stack, 0, pmm_nframes*8);
	
	if (start & 0xFFF) 
	{ 
		start &= 0xFFFFFFFFFFFFF000;
		start += 0x1000;
	}
	
	int i = entries;
	while (i)
	{
		if (mmap->type == 1)
		{
			if (mmap->base < start)
			{
				if (mmap->size > start + 0x1000)
				{
					mmap->size -= start;
					mmap->base	= start;
				}
				else
				{
					mmap->size = 0;
				}
			}
			if (mmap->base & 0xFFF)
			{
				mmap->base &= 0xFFFFFFFFFFFFF000;
				mmap->base += 0x1000
			}
			mmap->size /= 0x1000;

			while (mmap_size)
			{
				pmm_free_page(mmap->base);
				mmap->base += 0x1000;
				mmap_size -= 1;
			}
		}
		mmap++;
		i--;
	}
}

void pmm_free_page(uint64_t address)
{
	if (!pmm_test_bmap(address))
	{
		pmm_bclear(address);
		*pmm_stack = address;
		pmm_stack--;
	}
}

uint64_t pmm_alloc_page(void)
{
	pmm_stack++;
	uint64_t address = *pmm_stack;
	pmm_bset(address);
	return address;
}

uint64_t pmm_test_bmap(uint64_t address)
{
	uint8_t i = address / 0x1000;
	uint8_t b = 1 << (i % 64);
	return (pmm_bmap[i / 64] &= b);
}

void pmm_bset(uint64_t address)
{
	uint8_t i = address / 0x1000;
	uint8_t b = 1 << (i%64);
	pmm_bmap[i / 64] |= b;
}

void pmm_bclear(uint64_t address)
{
	uint8_t i = address / 0x1000;
	uint8_t b = 1 << (i % 64);
	pmm_bmap[i / 64] &= ~b;
}