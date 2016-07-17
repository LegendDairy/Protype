/* Pro-Type Kernel v1.3		*/
/* Kernel Entry Point		*/
/* By LegendMythe		*/

#include <text.h>
#include <idt.h>
#include <apic.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <ipl.h>
#include <pmm.h>
#include <heap.h>

int main(ipl_info_t *info)
{

	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	printf("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);
	printf("Kernel end at %x, Memorymap at %x, number of entries: %d, memsz %x, magic %x, low mem %x, high mem %x\n", (uint64_t)&end, info->mmap, info->mmap_entries, info->mem_sz, info->magic, info->low_mem, info->high_mem);
	init_idt();
	setup_pmm(info);
	setup_apic();

	int *ptr =  malloc(0x1000);
	*ptr = 5;
	printf("\nAllocating 0x1000, malloc returns: %x, %x", ptr, *ptr);

	int *ptr1 =  malloc(0x100);
	*ptr1 = 1;
	printf("\nAllocating 0x100, malloc returns: %x, %x", ptr1, *ptr1);
	
	int *ptr2 =  malloc(0x3200);
	*ptr2 = 2;
	printf("\nAllocating 0x3200, malloc returns: %x, %x", ptr2, *ptr2);
	
	view_heap();

	/*int *ptr3 =  malloc(0x6854);
	*ptr3 = 3;
	printf("\nAllocating 0x6854, malloc returns: %x, %x", ptr3, *ptr3);

	/*free((void*)ptr);
	printf("1");
	free((void*)ptr1);
	printf("1");
	free((void*)ptr2);
	printf("1");
	free((void*)ptr3);
	*/
	asm volatile("sti");
	
	for (;;);

	return 0;
}
