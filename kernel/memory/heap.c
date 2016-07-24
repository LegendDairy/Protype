/* Pro-Type Kernel v1.3		*/
/* Dynamic Memory Manager 1.1b	*/
/* By LegendMythe		*/

#include <heap.h>

/* NOT properly tested, shit is like some wanky Jenga-tower. */

volatile header_t *heap_start	= 0;												// Start of the heap (virtual address)
volatile uint64_t heap_end	= HEAP_START;											// End of the heap (virtual address)
uint64_t vmm_test_mapping(uint64_t va);

/* Internal function, shouldn't be called from outside!*/
void expand_heap (uint64_t start, uint64_t len);
static void split_chunk(header_t *chunk, uint64_t sz);										// Splits a chunk in two.
static void free_chunk (header_t *chunk);											// Frees a chunk (must be the last chunk of the heap).
static void glue_chunk (header_t *chunk);											// Glues the chunk to surounding chunks if possible.

/** Simple dynamic allocater for the kernel. **/
void *malloc(uint64_t sz)
{
	header_t *cur_header = (header_t *)heap_start, *prev_header = 0;

	while(cur_header)
	{
		if(cur_header->allocated == 0 && cur_header->size >= sz)
		{
			split_chunk(cur_header, sz);
		        cur_header->allocated = 1;
		        return (void*) ((uint64_t)cur_header + (uint64_t)sizeof(header_t));
		}

		prev_header 	= cur_header;
		cur_header 	= cur_header->next;
	}

	uint64_t chunk_start;

        if (prev_header)
	{
		chunk_start 	= (uint64_t)prev_header + (uint64_t)prev_header->size + (uint64_t)sizeof(header_t);
	}
        else
        {
          chunk_start 		= (uint64_t)HEAP_START;
          heap_start 		= (header_t *)chunk_start;
        }

        expand_heap(chunk_start, (uint64_t)sz + (uint64_t)sizeof(header_t));
	cur_header 		= (header_t *)chunk_start;
	cur_header->allocated 	= 1;
	cur_header->size 	= sz;
	cur_header->next 	= 0;
	cur_header->prev 	= prev_header;
	cur_header->prev->next 	= cur_header;
	cur_header->magic 	= MAGIC;
	return (void*)((uint64_t)cur_header + (uint64_t)sizeof (header_t));
}

/** Frees an allocated address. **/
void free(void *p)
{
	header_t *header 	= (header_t*)((uint64_t)p - (uint64_t)sizeof(header_t));	// Find the header.
	header->allocated 	= 0;													// Deallocate the chunk.
	glue_chunk(header);													// Glue chunks and possibly contract
}

void expand_heap(uint64_t start, uint64_t len)
{
	while (start + len > heap_end)
	{
		if(!vmm_test_mapping(heap_end))
		{
			vmm_map_frame((uint64_t)heap_end, (uint64_t)pmm_alloc_page, 0x3);
		}
		heap_end += 0x1000;
	}
}

/** Splits a chunk and addapts headers. **/
void split_chunk(header_t *chunk, uint64_t sz)
{
	/* Should we split the chunk? */
	if(chunk->size > ((uint64_t)sz+(uint64_t)sizeof(header_t) ))
	{
	/* Make new header. */
	header_t *new_chunk		= (header_t*)((uint64_t)chunk + (uint64_t)sizeof(header_t) + (uint64_t)sz);		// Create new chunk*
	new_chunk->allocated		= 0;											// Not allocated
	new_chunk->size			= (uint64_t)((uint64_t)chunk->size - (uint64_t)sz - (uint64_t)sizeof(header_t));
	new_chunk->magic		= MAGIC;										// Fill in magic code.
	new_chunk->next			= chunk->next;										// Pointer to next chunk.
	new_chunk->prev			= chunk;										// Pointer to previous chunk.

		if(new_chunk->next)
		{
			new_chunk->next->prev = new_chunk;
		}

	/* Addapt previous header. */
	chunk->next 	 	= new_chunk;												// Pointer to new chunk.
	chunk->size		= sz;												// Change size.
	}
}

/* Contracts the end of the heap. */
void free_chunk(header_t *chunk)
{
	/* Adapt previous header. */
	if(chunk->prev)
	{
		chunk->prev->next = 0;
	}
  	else
	{
		heap_start = 0;
	}

	/* While the heap max can contract by a page and still be greater than the chunk address. */
	while ( (heap_end-0x1000) >= (uint64_t)chunk )
	{
	  heap_end -= 0x1000;
	  pmm_free_page(vmm_get_mapping(heap_end, 0));										// Free the frame.
	  vmm_unmap_frame(heap_end);												// Unmap the virtual address.
	}
}

/* Glues two chunk and contracts the heap. */
void glue_chunk (header_t *chunk)
{
	/** There's a chunk after this one, glue them. **/
	if (chunk->next && chunk->next->allocated == 0)
	{
		chunk->size += (chunk->next->size + sizeof(header_t));						// Change size.
		chunk->next = chunk->next->next;								// Change pointer to next chunk
		if(chunk->next)
		{
			chunk->next->prev = chunk;
		}
	}

	/** There's a chunk before this one, glue them. **/
	if (chunk->prev && chunk->prev->allocated == 0)
	{
		chunk->prev->size += (chunk->size + sizeof(header_t));								// Change size.
		chunk->prev->next = chunk->next;										// Change pointer to next chunk.
		if (chunk->next)
		{
			chunk->next->prev = chunk->prev;
		}	  							// Change pointer of the next chunk to this one.
		chunk = chunk->prev;												// Change starting address.
	}

	/** There's no chunk after this one, free it. **/
	if (chunk->next == 0)
	{

		free_chunk(chunk);												// Contract.
	}
}

/* Routine to check for heap corruption. */
uint64_t check_heap(void)

{
	header_t *iterator = (header_t *)heap_start;
	while(iterator)
	{
		if(iterator->magic)
		{
			/* !Magic code is corrupted! */
			DebugSetTextColour(0xF,0);
			printf("\n[");
			DebugSetTextColour(0x4,0);
			printf("HEAP");
			DebugSetTextColour(0xF,0);
			printf("]:");
			printf("Error 0x04a: Heap corrupted! Magic code corrupt!\n");
			return 1;
		}
		iterator = iterator->next;
	}
	return 0;
}

void view_heap(void)
{

	if(heap_start)
	{
		printf("\nHeap start at: %x, size: %x, allocated: %d", heap_start, heap_start->size, heap_start->allocated);
		header_t *iterator = heap_start->next;

		while(iterator)
		{
			printf("\nNext entry at %x, size: %x, allocated: %d", iterator, iterator->size, iterator->allocated);
			iterator = iterator->next;
		}
	}
	else
	{
		printf("\nNo Heap!");
	}

	printf("\nHeap_end at %x", heap_end);
}
