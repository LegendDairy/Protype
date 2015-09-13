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
	printf("Allocating 5kb, malloc returns: %x", malloc(0x5000));
	
	//asm volatile("sti");
	
	for (;;);

	return 0;
}
