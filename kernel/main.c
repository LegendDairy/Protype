/* Pro-Type Kernel v1.3		*/
/* Kernel Entry Point		*/
/* By LegendMythe		*/

#include <text.h>

int main(uint64_t *memorymap, uint64_t map_entries)
{

	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	DebugPuts("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);
	DebugPutHex(memorymap);
	DebugPuts("\n");
	DebugPutHex(map_entries);
	init_idt();
	setup_apic();
	
	//asm volatile("sti");
	
	for (;;);
	while(1);
	return 0;
}
