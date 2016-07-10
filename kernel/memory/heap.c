/* Pro-Type Kernel v1.3			*/
/* Dynamic Memory Manager 1.1	*/
/* By LegendMythe				*/

#include <heap.h>

/* NOT properly tested, shit is like some wanky Jenga-tower. */

volatile header_t *heap_start	= 0;											// Start of the heap (virtual address)
volatile uint64_t heap_end		= 0;											// End of the heap (virtual address)

/* Internal function, shouldn't be called from outside!*/
static header_t *create_chunk(uint64_t sz);										// Expands the heap.
static void split_chunk(header_t *chunk, uint64_t sz);							// Splits a chunk in two.
static void free_chunk (header_t *chunk);										// Frees a chunk (must be the last chunk of the heap).
static void glue_chunk (header_t *chunk);										// Glues the chunk to surounding chunks if possible.

/** Simple dynamic allocater for the kernel **/
void *malloc(uint64_t sz)
{
	/* Heap hasn't been initialised, create a simple block of 4k. */
	if(heap_start == 0)
	{
		/* Create a heap. */
		heap_start	= (header_t *)HEAP_START;												// Set the virtual address of the heap.
		heap_end	= (uint64_t)(HEAP_START + 0x1000);									// Set address of the end.
		vmm_map_frame((uint64_t)HEAP_START, (uint64_t)pmm_alloc_page, 0x3);							// Map the first entry.

		/* Fill in the first header. */
		heap_start->allocated = 0;												// Not allocated
		heap_start->next = 0;													// No chunks after this.
		heap_start->prev = 0;													// First chunk of the heap
		heap_start->magic0 = MAGIC;												// Set the magic code
		heap_start->magic1 = MAGIC;												// Set the magic code
		heap_start->size = (0x1000 - sizeof(header_t));							// Set the size of the chunk.
	}

	/* Iterate through the linked list to search for a free chunk. */
	header_t *iterator = (header_t *)heap_start;
	while(iterator)
	{
		/* Magic code is corrupted? */
		if(iterator->magic0 != MAGIC || iterator->magic1 != MAGIC)
		{
			DebugSetTextColour(0xF,0);
			printf("\n[");
			DebugSetTextColour(0x4,0);
			printf("HEAP");
			DebugSetTextColour(0xF,0);
			printf("]:");
			printf("Error 0x04a: Heap corrupted!: Magic code error\n");
		}
		/* Does the chunk fit perfectly? */
		if(iterator->size == sz && iterator->allocated == 0)
		{
			iterator->allocated = 1;											// Chunk is now allocated.
			return (void *)((uint64_t)iterator + (uint64_t)sizeof(header_t));						// Return the starting address.
		}
		/* Does the chunk fit? */
		if(iterator->size > sz && iterator->allocated == 0)
		{
			split_chunk(iterator, sz);											// Split the chunk.
			iterator->allocated = 1;											// Chunk is now allocated.
			return (void *)((uint64_t)iterator + (uint64_t)sizeof(header_t));						// Return the starting address.
		}
		iterator = iterator->next;												// Iterate
	}

	/* No fitting holes found! Expand.	*/
	header_t *chunk = (header_t *)create_chunk(sz);								// Create a new chunk.
	split_chunk(chunk, sz);														// Make the chunk fit
	chunk->allocated = 1;														// Chunk is now allocated.
	return (void *)((uint64_t)chunk + (uint64_t)sizeof(header_t));									// Return the starting address.
}

/** Frees an allocated address. **/
void free(void *p)
{
	header_t *header = (header_t*)((uint64_t)p - (uint64_t)sizeof(header_t));	// Find the header.
	header->allocated = 0;														// Deallocate the chunk.
	glue_chunk(header);															// Glue chunks and possibly contract
}

/** Splits a chunk and addapts headers. **/
void split_chunk(header_t *chunk, uint64_t sz)
{
	/* Should we split the chunk? */
	if(chunk->size > ((uint64_t)sz+(uint64_t)sizeof(header_t) + 1))
	{
									// Start of new chunk

	/* Make new header. */
	header_t *new_chunk		= (header_t*)((uint64_t)chunk + (uint64_t)2*sizeof(header_t) + (uint64_t)sz);												// Create new chunk
	new_chunk->allocated	= 0;												// Not allocated
	new_chunk->size			= (uint64_t)((uint64_t)chunk->size - (uint64_t)sz - (uint64_t)sizeof(header_t)));
	new_chunk->magic0		= MAGIC;											// Fill in magic code.
	new_chunk->magic1		= MAGIC;											// Fill in magic code.
	new_chunk->next			= chunk->next;										// Pointer to next chunk.
	new_chunk->prev			= chunk;											// Pointer to previous chunk.

if(new_chunk->next)
{
	new_chunk->next->prev = new_chunk;

}

	/* Addapt previous header. */
	chunk->next 	 = new_chunk;												// Pointer to new chunk.
	chunk->size		 = sz;														// Change size.
	}
}

/* Expands the heap. */
header_t *create_chunk(uint64_t sz)
{
	sz += (uint64_t)sizeof(header_t);														// Add header size

	/*Iterate, find the last block of the heap. */
	header_t *iterator = (header_t *)heap_start;
	while(iterator->next)
	{
		iterator = iterator->next;
	}

	header_t *chunk = (header_t *)heap_end;													// Create a chunk
	iterator->next = chunk;														// Pointer to new block
	uint64_t i=0;

	/* Block will atleast be 0x1000 bytes bigger. */
	for(i = 0;i<((sz / 0x1000)+1);i++)
	{
	vmm_map_frame((uint64_t)heap_end, (uint64_t)pmm_alloc_page, 0x3);								// Map a new physical frame for the heap
	heap_end += 0x1000;															// Adjust heap end var
	}
	printf(" i=%x ", i);
	/* Fill in the new header. */
	chunk->size = ((uint64_t)(0x1000*i) - (uint64_t)sizeof(header_t));								// Size of the block
	chunk->magic0 = MAGIC;														// Magic code
	chunk->magic1 = MAGIC;														// Second Magic code
	chunk->next = 0;															// Last block of the heap
	chunk->prev = iterator;														// Pointer to previous block
	chunk->allocated = 0;														// Block is not allocated


	/* Return the new chunk. */
	return chunk;
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
	  pmm_free_page(vmm_get_mapping(heap_end, 0));								// Free the frame.
	  vmm_unmap_frame(heap_end);												// Unmap the virtual address.
	}
/* Change size of remaining chunk! */
}

/* Glues two chunk and contracts the heap. */
void glue_chunk (header_t *chunk)
{
		printf(" %x ", chunk);
	/** There's a chunk after this one, glue them. **/
	if(chunk->next)
	{
		if(!chunk->next->allocated)
		{
		printf(" %xi ", chunk->next);

				chunk->size += (chunk->next->size + sizeof(header_t));			// Change size.
				chunk->next = chunk->next->next;								// Change pointer to next chunk
				if(chunk->next)  chunk->next->prev = chunk;			// change pointer of the next chunk to this one.
		}
	}

	/** There's a chunk before this one, glue them. **/
	if(chunk->prev)
	{
		if(!chunk->prev->allocated)
		{
				printf("e");
		chunk->prev->size += (chunk->size + sizeof(header_t));					// Change size.
		chunk->prev->next = chunk->next;										// Change pointer to next chunk.
		if (chunk->next) chunk->next->prev = chunk->prev;	  					// Change pointer of the next chunk to this one.
		chunk = chunk->prev;													// Change starting address.
		}
	}

	/** There's no chunk after this one, free it. **/
	if (chunk->next == 0)
	{

		free_chunk(chunk);														// Contract.
	}
}

/* Routine to check for heap corruption. */
uint64_t check_heap(void)

{
	header_t *iterator = (header_t *)heap_start;
	while(iterator)
	{
		if(iterator->magic0 != MAGIC || iterator->magic1 != MAGIC)
		{
			/* !Magic code is corrupted! */
			DebugSetTextColour(0xF,0);
			printf("\n[");
			DebugSetTextColour(0x4,0);
			printf("HEAP");
			DebugSetTextColour(0xF,0);
			printf("]:");
			printf("Error 0x04b: Heap corrupted! Magic code corrupt!\n");
			return 1;
		}
		/* Iterate */
		iterator = iterator->next;
	}
	return 0;
}
