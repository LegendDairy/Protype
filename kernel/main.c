/* Pro-Type Kernel v1.3		*/
/* Kernel Entry Point		*/
/* By LegendMythe		*/

#include <text.h>

int main(void)
{

	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	DebugPuts("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);
	
	init_idt();
	setup_apic();
	
	asm volatile("sti");
	
	for (;;);

	return 0;
}
