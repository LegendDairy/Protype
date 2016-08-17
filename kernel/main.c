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
#include <scheduler.h>

void parse_madt(void);

/* TODO */
/* -Setup a proper kernel stack. 				*/
/* -Maybe at page-boundries to the stack for overrun detection? */
/* -Clean up debug-text code. 					*/
/* -Graphics proof of concept.					*/


uint64_t locker = 0;
void 	tm_sched_kill_current_thread(void);
void tm_schedule_sleep(uint64_t);
int thread(uint64_t argn, char **argv)
{
	asm("cli");
	register processor_t *curr asm("r12") =  system_info_get_current_cpu();
	asm("sti");
	while(!curr->current_thread);
	printf("Hello from %s running on logical cpu %x. Argn: %x, Argv: %x\n", curr->current_thread->name, curr->apic_id, argn, (uint64_t)argv);
	tm_schedule_sleep(1000);
	//curr =  system_info_get_current_cpu();
	printf("Woke up!\n");
	while(1);
	return 0xDEADBEEF;
}

int main(ipl_info_t *info)
{
	DebugClearScreen();
	DebugSetTextColour(0x2, 0);
	printf("Protype v1.3\n");
	DebugSetTextColour(0xF, 0);

	init_idt();
	setup_pmm(info);
	setup_vmm();
	parse_madt();
	setup_apic();

	/* Interrupts shouldn't be enabled before setup_tm! */
	setup_tm();

	/* Proof of concept: preemptive SMP support: */
	boot_ap(1);
	boot_ap(2);
	boot_ap(3);

	vmm_map_frame(0x90000000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90001000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90002000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90003000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90004000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90005000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0x90006000, pmm_alloc_page(), 0x3);
	vmm_map_frame(0xA0000000, pmm_alloc_page(), 0x3);


	tm_thread_create(&thread, 1,  (char **)1,  0x10000, 1, 100, "Thread 1", 1, (uint64_t *)0x90000F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread, 0, 0, 0x10000, 1, 100, "Thread 2", 1,  (uint64_t *)0x90001F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread, 0, 0,  0x10000, 1, 100, "Thread 3", 1,  (uint64_t *) (uint64_t *)0x90002F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread, 0, 0,  0x10000, 1, 100, "Thread 4", 1,  (uint64_t *)0x90003F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread, 0, 0, 0x10000, 1, 100, "Thread 5", 1,  (uint64_t *)0x90004F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread, 0, 0,  0x10000, 1, 100, "Thread 6", 1,  (uint64_t *)0x90005F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread, 0, 0,  0x10000, 1, 100, "Thread 7", 1,  (uint64_t *)0x90006F00, 0x10, 0x8, 0x10);
	tm_thread_create(&thread, 0, 0, 0x10000, 1, 100, "Thread 8", 1,  (uint64_t *)0xA0000F00, 0x10, 0x8, 0x10);

	asm volatile("sti");
	//tm_sched_kill_current_thread();
	while(1)
	{
		//asm volatile("hlt");
	}

	return 0;
}


/*uint32_t *ptr1 = malloc(0x500);
*ptr1 = 0xDEADBEE1;
uint32_t *ptr2 = malloc(0x1100);
*ptr2 = 0xDEADBEE2;
uint32_t *ptr3 = malloc(0x400);
*ptr3 = 0xDEADBEE3;
uint32_t *ptr4 = malloc(0x3200);
*ptr4 = 0xDEADBEE4;
uint32_t *ptr5 = malloc(0x50);
*ptr5 = 0xDEADBEE5;
uint32_t *ptr6 = malloc(0x10);
*ptr6 = 0xDEADBEE6;
uint32_t *ptr7 = malloc(0x500);
*ptr7 = 0xDEADBEE7;
uint32_t *ptr8 = malloc(0x11100);
*ptr8 = 0xDEADBEE8;
uint32_t *ptr9 = malloc(0x414);
*ptr9 = 0xDEADBEE9;
uint32_t *ptrA = malloc(0xFF000);
*ptrA = 0xDEADBEEA;
uint32_t *ptrB = malloc(0x52);
*ptrB = 0xDEADBEEB;
uint32_t *ptrC = malloc(0x11);
*ptrC = 0xDEADBEEC;

printf("Malloc 1 returned: %x, for size 0x500, ptr returned: %x\n", 	ptr1, *ptr1);
printf("Malloc 2 returned: %x, for size 0x1100, ptr returned: %x\n", 	ptr2, *ptr2);
printf("Malloc 3 returned: %x, for size 0x400, ptr returned: %x\n",	ptr3, *ptr3);
printf("Malloc 4 returned: %x, for size 0x3200, ptr returned: %x\n", 	ptr4, *ptr4);
printf("Malloc 5 returned: %x, for size 0x50, ptr returned: %x\n", 	ptr5, *ptr5);
printf("Malloc 6 returned: %x, for size 0x10, ptr returned: %x\n", 	ptr6, *ptr6);

printf("Malloc 7 returned: %x, for size 0x500, ptr returned: %x\n", 	ptr7, *ptr7);
printf("Malloc 8 returned: %x, for size 0x1100, ptr returned: %x\n", 	ptr8, *ptr8);
printf("Malloc 9 returned: %x, for size 0x400, ptr returned: %x\n",	ptr9, *ptr9);
printf("Malloc A returned: %x, for size 0x3200, ptr returned: %x\n", 	ptrA, *ptrA);
printf("Malloc B returned: %x, for size 0x50, ptr returned: %x\n", 	ptrB, *ptrB);
printf("Malloc C returned: %x, for size 0x10, ptr returned: %x\n", 	ptrC, *ptrC);*/
