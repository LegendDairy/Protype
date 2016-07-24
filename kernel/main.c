/* Pro-Type Kernel v1.3		*/
/* Kernel Module		*/
/* By LegendMythe		*/

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <text.h>
#include <ipl.h>
#include <pmm.h>
#include <vmm.h>
#include <heap.h>
#include <apic.h>
#include <idt.h>

/* TODO */
/* -Setup a proper kernel stack. 				*/
/* -Maybe at page-boundries to the stack for overrun detection? */
/* -Clean up debug-text code. 					*/
/* -Graphics proof of concept.					*/

int main(ipl_info_t *info)
{
	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	printf("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);
	init_idt();
	setup_pmm(info);
	setup_vmm();
	setup_apic();

	asm volatile("sti");
	for (;;);

	return 0;
}
