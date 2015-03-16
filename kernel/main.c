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
	DebugPuts("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);
	DebugPutHex((uint64_t)&end);
	DebugPuts("\n");
	DebugPutHex((uint64_t)memorymap);
	DebugPuts("\n");
	DebugPutHex(map_entries);
        DebugPuts("\n");
	printf("%s %d %x","Hello World of Formatted Strings!\n", (uint64_t)5, 6);
	init_idt();
	setup_apic();
	
	//asm volatile("sti");
	
	for (;;);
	while(1);
	return 0;
}
