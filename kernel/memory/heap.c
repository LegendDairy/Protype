/* Pro-Type Kernel v1.3		*/
/* Dynamic Memory Manager 1.1	*/
/* By LegendMythe		*/

#include <heap.h>

/* Headers that point to the start and end. */
header_t *heap_start	= 0;
header_t *heap_end		= 0;

/* Internal function, shouldn't be called from outside!*/
<<<<<<< HEAD
static header_t *create_chunk(uint64_t sz);
static void split_chunk(header_t *chunk, uint64_t sz);
=======
static header_t *create_chunk(u32int sz);
static void split_chunk(header_t *chunk, u32int sz);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
static void free_chunk (header_t *chunk);
static void glue_chunk (header_t *chunk);

/** Dynamic allocater, chrashes if you overwrite your chunk! **/
<<<<<<< HEAD
void *malloc(uint64_t sz)
=======
void *malloc(u32int sz)
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
{
	/* Heap hasn't been initialised, create a simple block of 4k. */
	if(heap_start == 0)
	{
		/* Map the first entry. */
<<<<<<< HEAD
		vmm_map_frame(HEAP_START, pmm_alloc_page, 0x3);
=======
		map(HEAP_START, pmm_alloc_page, 0x3);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
		
		/* Set the variables. */
		heap_start	= HEAP_START;
		heap_end	= (HEAP_START + 0x1000);

		/* Fill in the first header. */
		heap_start->allocated = 0;
		heap_start->next = 0;
		heap_start->prev = 0;
		heap_start->magic0 = MAGIC;
		heap_start->magic1 = MAGIC;
		heap_start->size = (0x1000 - sizeof(header_t));
	}

	/* Iterate through the linked list. */
	header_t *iterator = heap_start;
	while(iterator)
	{
		if(iterator->magic0 != MAGIC || iterator->magic1 != MAGIC)
		{
			/* !Magic code is corrupted! */
<<<<<<< HEAD
			DebugSetTextColour(0xF,0);
			printf("\n[");
			DebugSetTextColour(0x4,0);
			printf("HEAP");
			DebugSetTextColour(0xF,0);
=======
			settextcolour(0xF,0);
			printf("\n[");
			settextcolour(0x4,0);
			printf("HEAP");
			settextcolour(0xF,0);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
			printf("]:");
			printf("Error 0x04: Heap corrupted!\n");
		}
		/* Block fits perfectly! */
		if(iterator->size == sz && iterator->allocated == 0)
		{
			/* Found block that fits! */
			iterator->allocated = 1;

			void *addr = (void *)iterator;
			addr += sizeof(header_t);
			return addr;
		}
		/* Block fits, but has extra space. */
		if(iterator->size > sz && iterator->allocated == 0)
		{
			/* Found a large block. */
			split_chunk(iterator, sz);
			iterator->allocated = 1;

			/* Get the starting address */
			void *addr = (void *)iterator;
			addr += sizeof(header_t);
			return addr;
		}
		/* Iterate */
		iterator = iterator->next;
	}

	/* No fitting holes found! Expand.	*/
<<<<<<< HEAD
	header_t *chunk = (header_t *)create_chunk(sz);
=======
	header_t *chunk = create_chunk(sz);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
	/* Make the chunk fit perfectly.	*/
	split_chunk(chunk, sz);
	/* Allocate it!						*/
	chunk->allocated = 1;

	/* Get the starting address */
	void *addr = (void *)chunk;
	addr += sizeof(header_t);
	return addr;
}

/** Frees an allocated address. **/
void free(void *p)
{
	/* Find the header. */
<<<<<<< HEAD
	uint64_t address = (uint64_t)p - (uint64_t)sizeof(header_t);
=======
	u32int address = (u32int)p - (u32int)sizeof(header_t);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
	header_t *header = (header_t*)address;
	
	/* Deallocate */
	header->allocated = 0;
	/* Glue the all the chunks to getter and shrink */
	glue_chunk(header);
}

/** Splits a chunk and addapts headers. **/
<<<<<<< HEAD
void split_chunk(header_t *chunk, uint64_t sz)
=======
void split_chunk(header_t *chunk, u32int sz)
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
{
	/* Is it worth splitting it? */
	if(chunk->size > (sz+sizeof(header_t)) )
	{
	/* Find the address of the new chunk */
<<<<<<< HEAD
	uint64_t addr = (header_t*)(chunk);
	addr += (uint64_t)sizeof(header_t);
=======
	u32int addr = (header_t*)(chunk);
	addr += (u32int)sizeof(header_t);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
	addr += sz;

	/* Create new chunk. */
	header_t *new_chunk = addr;

	/* Reset magic code */
	chunk->magic0 = MAGIC;
	chunk->magic1 = MAGIC;
	
	/* Fill in the headers */
	new_chunk->allocated	= 0;
<<<<<<< HEAD
	new_chunk->size			= (uint64_t)((uint64_t)chunk->size - ((uint64_t)sz + (uint64_t)sizeof(header_t)));
=======
	new_chunk->size			= (u32int)((u32int)chunk->size - ((u32int)sz + (u32int)sizeof(header_t)));
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
	new_chunk->magic0		= MAGIC;
	new_chunk->magic1		= MAGIC;
	new_chunk->next			= chunk->next;
	new_chunk->prev			= chunk;

	/* Addapt previous header. */
	chunk->next 	 = new_chunk;
	chunk->size		 = sz;
	}
}

/* Expands the heap. */
<<<<<<< HEAD
header_t *create_chunk(uint64_t sz)
=======
header_t *create_chunk(u32int sz)
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
{
	/* Add header size! */
	sz += sizeof(header_t);
	
	/*Iterate, find last header. */
	header_t *iterator = heap_start;
	while(iterator->next)
	{
		iterator = iterator->next;
	}

	/* Create a chunk. */
	header_t *chunk = heap_end;
	/* Addapt previous header. */
	iterator->next = chunk;
	
<<<<<<< HEAD
	uint64_t i;
=======
	u32int i;
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
	/* Block will atleast be 0x1000 bytes bigger. */
	for(i=0;i<((sz / 0x1000)+1);i++)
	{
	/* Create a virtual address for the heap. */
	vmm_map_frame((heap_end + (0x1000*i)), pmm_alloc_page, 0x3);
	}

	/* Fill in the new header. */
	chunk->size = ((0x1000*i) - sizeof(header_t));
	chunk->magic0 = MAGIC;
	chunk->magic1 = MAGIC;
	chunk->next = 0;
	chunk->prev = iterator;
	chunk->allocated = 0;								
	
	/* Change end variable. */
	i = i * 0x1000;
<<<<<<< HEAD
	heap_end = (uint64_t)heap_end + (i);				
=======
	heap_end = (u32int)heap_end + (i);				
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422

	/* Return the new chunk. */
	return chunk;
}

/* Contracts the heap. */
void free_chunk(header_t *chunk)
{
	/* Adapt previous header. */
	if(chunk->prev)										
	{
	chunk->prev->next = 0;
	}

	/* Errase the whole heap. */
	if (chunk->prev == 0)
	{
	heap_start = 0;
	}

	/* While the heap max can contract by a page and still be greater than the chunk address. */
<<<<<<< HEAD
	while ( (heap_end-0x1000) >= (uint64_t)chunk )
=======
	while ( (heap_end-0x1000) >= (uint32_t)chunk )
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
	{
	  /* Heap end is 0x1000 smaller. */
	  heap_end -= 0x1000;
	  
	  /* Get physical address. */
<<<<<<< HEAD
	  uint64_t page;
	  vmm_get_mapping (heap_end, &page);
=======
	  u32int page;
	  get_mapping (heap_end, &page);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422

	  /* Free the physical page. */
	  pmm_free_page (page);
	  /* Unmap the virtual address. */
<<<<<<< HEAD
	  vmm_unmap_frame (heap_end);
=======
	  unmap (heap_end);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
	}
}

/* Glues two chunk and contracts the heap. */
void glue_chunk (header_t *chunk)
{
	/** There's a chunk after this one, glue them. **/
	if(chunk->next)
	{
		if(chunk->next->allocated == 0)
		{

				chunk->size += (chunk->next->size + sizeof(header_t));
				chunk->next = chunk->next->next;
				if(chunk->next->next) chunk->next->next->prev = chunk;
		}
	}
	
	/** There's a chunk before this one, glue them. **/
	if(chunk->prev)
	{
		if(chunk->next->allocated == 0)
		{
		chunk->prev->size += (chunk->size + sizeof(header_t));
		chunk->prev->next = chunk->next;
		if(chunk->next) chunk->next->prev = chunk->prev;
		chunk = chunk->prev;
		}
	}
	/** There's no chunk after this one, free it. **/
	if (chunk->next == 0)
	{
		/* Contract. */
		free_chunk(chunk);
	}
}

<<<<<<< HEAD
uint64_t check_heap(void)
=======
u32int check_heap(void)
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
{
	header_t *iterator = heap_start;
	while(iterator)
	{
		if(iterator->magic0 != MAGIC || iterator->magic1 != MAGIC)
		{
			/* !Magic code is corrupted! */
<<<<<<< HEAD
			DebugSetTextColour(0xF,0);
			printf("\n[");
			DebugSetTextColour(0x4,0);
			printf("HEAP");
			DebugSetTextColour(0xF,0);
=======
			settextcolour(0xF,0);
			printf("\n[");
			settextcolour(0x4,0);
			printf("HEAP");
			settextcolour(0xF,0);
>>>>>>> 31c8e74bcd477645971f6257b3be365c96210422
			printf("]:");
			printf("Error 0x04: Heap corrupted!\n");
			return 1;
		}
		/* Iterate */
		iterator = iterator->next;
	}
	return 0;
}
