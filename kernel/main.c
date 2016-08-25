/* Pro-Type Kernel v0.2		*/
/* Main Entry of Kernel Module	*/
/* By LegendDairy		*/

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <text.h>
#include <ipl.h>
#include <pmm.h>
#include <vmm.h>
#include <heap.h>
#include <apic.h>
#include <acpi.h>
#include <idt.h>
#include <scheduler.hpp>
#include <system.hpp>

/* TODO */
/* -Setup a proper kernel stack. 				*/
/* -Maybe at page-boundries to the stack for overrun detection? */
/* -Clean up debug-text code. 					*/
/* -Graphics proof of concept.					*/

int thread1(uint64_t argn, char **argv)
{
	while(1)
	{
		printf("a");
		tm_thread_sleep(10);
	}

	return 0xDEADBEEF;
}

int thread2(uint64_t argn, char **argv)
{
	while(1)
	{
		printf("b");
		tm_thread_sleep(10);
	}

	return 0xDEADBEEF;
}

int thread3(uint64_t argn, char **argv)
{
	while(1)
	{
		printf("c");
		tm_thread_sleep(10);
	}

	return 0xDEADBEEF;
}

int main(ipl_info_t *info)
{
	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	printf("Protype v0.2\n");
	DebugSetTextColour(0xF, 0);

	init_idt();
	setup_pmm(info);
	setup_vmm();
	system_c::setup();

	vmm_map_frame(0x90000000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90001000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90002000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90003000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90004000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90005000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90006000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90007000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90008000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90009000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x9000A000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x9000B000, pmm_alloc_page(), 0x3);

	tm_thread_create(&thread1, 0, 0, 0x10000, 1, 30, "Thread 1", 1, (uint64_t *)0x90000F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread2, 0, 0, 0x10000, 1, 30, "Thread 2", 1, (uint64_t *)0x90001F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread3, 0, 0, 0x10000, 2, 30, "Thread 3", 1, (uint64_t *)0x90002F00, 0x10, 0x8, 0x10);

	asm volatile("sti");
	tm_kill_current_thread();
	while(1);

	return 0;
}
