/* Pro-Type Kernel v1.3		*/
/* Kernel Entry Point		*/
/* By LegendMythe		*/

#include <text.h>
#include <idt.h>
#include <apic.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>


extern uint64_t end;

int main(uint64_t *memorymap, uint64_t map_entries)
{

	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	printf("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);
	printf("Kernel end at %x, Memorymap at %x, number of entries: %d\n", (uint64_t)&end, (uint64_t)memorymap, map_entries);
	init_idt();
	setup_apic();
	
	//asm volatile("sti");
	
	for (;;);

	return 0;
}
