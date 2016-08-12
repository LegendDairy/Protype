/* Pro-Type Kernel v1.3	*/
/* Advanced PIC	   v0.2	*/
/* By LegendMythe	*/

#include<apic.h>
#include<acpi.h>
#include<idt.h>

#define APB_BASE 0x50000

volatile uint64_t tick;
uint32_t apic_base;
extern idt_ptr_t idt_ptr;
extern topology_t *system_info;

uint8_t inb(uint16_t port)
{
    uint8_t byte;
    asm volatile("inb %1, %0":"=a"(byte): "dN" (port));
    return byte;
}

void pit_handler(void)
{
}

uint8_t apic_check(void)
{
	uint32_t eax, edx;
	asm volatile("cpuid":"=a"(eax),"=d"(edx):"a"(1):"ecx","ebx");
	return edx & CPUID_FLAG_APIC;
}

uint32_t lapic_read(uint32_t r)
{
	return ((uint32_t)(system_info->lapic_address[r / 4]));
}

void lapic_write(uint32_t r, uint32_t val)
{
	system_info->lapic_address[r / 4] = (uint32_t)val;
}

void setup_apic(void)
{
	/* Parse the multiprocessor table. */
	/* Address label for the asm code. */
	apic_base = (uint32_t *) system_info->lapic_address;
	/* Map the local apic to a virtual address. */
	vmm_map_frame((uint64_t) apic_base,(uint64_t) apic_base, 0x3);

        /* TODO: */
        /* -Map lapic and ioapic address here instead of in the ipl.    */
        /* -Clean up the code.                                          */
        /* -Separate ioapic and lapic code.                             */
        /* -Make some documentation.                                    */
        /* -Mask all IRQs (not the "cascade" line) in the PIC chips     */
        /* -Mask everything in all IO APICs.                            */
        /* Create an IDT with interrupt vectors for the local APIC's spurious IRQ,
        /* the master PIC's spurious IRQ and the slave PIC's spurious IRQ. */
        /* LINT0 and LINT1 should be setup dynamically (according to whatever
        the MPS table or ACPI says), rather than hardcoded. Harcoding them
        (like in the example in Intel manuals) is only really appropriate
        for BIOSs (where they know how the inputs are connected in advance). */
	//http://forum.osdev.org/viewtopic.php?p=107868#107868

	/* Set up Local APIC */
	lapic_write(apic_reg_task_priority, 0x00);			// Accept all interrupts
	lapic_write(apic_lvt_timer_reg, 0x10000);			// Disable timer interrupts
	lapic_write(apic_lvt_thermal_reg, 0x10000);			// Dissable Thermal monitor
	lapic_write(apic_lvt_perf_reg, 0x10000);			// Disable performance counter interrupts
	lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
	lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
	lapic_write(apic_lvt_error_reg, 0x10000);			// Disable error interrupts
        lapic_write(apic_reg_dest_format, 0xF0000000);               	// Flatmode
        lapic_write(apic_reg_logical_dest, 0xFF000000);			// Destination bits for this apic
	lapic_write(apic_reg_spur_int_vect, 0x0013F);			// Enable the APIC and set spurious vector to 0x3F
	lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
	lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
	lapic_write(apic_reg_processor_priority, 0x00);			// Enable normal NMI processing
	lapic_write(apic_reg_arbitration_priority, 0x00);		// Enable normal NMI processing
	lapic_write(apic_reg_eoi, 0x00);				// Make sure no interrupts are left


	/* Set up IO APIC for the PIT (POC) */
	/*uint32_t * volatile ioapic_reg 	= (uint32_t *)system_info->io_apic->address;
 	uint32_t * volatile ioapic_io 	= (uint32_t *)(system_info->io_apic->address+0x4);

	vmm_map_frame(ioapic_reg, ioapic_reg, 0x3); 			// Identity map io apic address
 	*(uint32_t*)ioapic_reg 	= (uint32_t)0x14;
 	*ioapic_io 		= (uint32_t)0x030 ;
 	*(uint32_t*)ioapic_reg 	= (uint32_t)0x15;
 	*ioapic_io 		= (uint32_t)0xFF000000;*/


	/* Set up LAPIC Timer. */
	setup_lapic_timer();
	//boot_ap(1);
}
void apic_ap_setup(void)
{
	printf("[SMP]: CPU %x is booting...\n", lapic_read(apic_reg_id) >> 24);
	lapic_write(apic_reg_task_priority, 0x00);			// Accept all interrupts
	lapic_write(apic_lvt_timer_reg, 0x10000);			// Disable timer interrupts
	lapic_write(apic_lvt_thermal_reg, 0x10000);			// Dissable Thermal monitor
	lapic_write(apic_lvt_perf_reg, 0x10000);			// Disable performance counter interrupts
	lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
	lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
	lapic_write(apic_lvt_error_reg, 0x10000);			// Disable error interrupts
        lapic_write(apic_reg_dest_format, 0xF0000000);               	// Flatmode
        lapic_write(apic_reg_logical_dest, 0xFF000000);			// Destination bits for this apic
	lapic_write(apic_reg_spur_int_vect, 0x0013F);			// Enable the APIC and set spurious vector to 0x3F
	lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
	lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
	lapic_write(apic_reg_processor_priority, 0x00);			// Enable normal NMI processing
	lapic_write(apic_reg_arbitration_priority, 0x00);		// Enable normal NMI processing
	lapic_write(apic_reg_eoi, 0x00);				// Make sure no interrupts are left

	setup_lapic_timer();
}
void setup_lapic_timer(void)
{
	/* Set LAPIC timer as reg int 48 */
	lapic_write(apic_lvt_timer_reg, 0x00030);      // int 48
	lapic_write(apic_div_conf, apic_timer_div_4);  // Divide by 4

	/* Set up PIT */
	outb(0x61, (inb(0x61) & 0xFD) | 1);            //
	outb(0x43,0xB2);                               //
	outb(0x42,0xa9);	                       // LSB (1193180/1000 Hz = 1193 = 0x04a9)
        inb(0x60);                                     // short delay
	outb(0x42,0x04);                               // MSB

	//reset PIT one-shot counter (start counting)
	outb(0x61,(uint8_t)inb(0x61)&0xFE);            //gate low
	outb(0x61,(uint8_t)(inb(0x61)&0xFE)|1);        //gate high

	lapic_write(apic_init_count, 0xFFFFFFFF);

	while(!(inb(0x61)&0x20));                      // sleep(1ms);

	/* Calculate divisor */
	lapic_write(apic_lvt_timer_reg, 0x10030);
	uint32_t freq = lapic_read(apic_cur_count);
	freq = 0xFFFFFFFF - freq;
	freq = freq*4/1000;

	/* Give information to the user */
	printf("[APIC]: Bus frequency:  %dMHz\n", freq);

	/* Setup intial count */
	lapic_write(apic_init_count, freq * 1000);                                    // Fire every micro second
	lapic_write(apic_lvt_timer_reg, (uint32_t)(0x20 | apic_timer_period)); //int 48, periodic
}

/* Proof of concept: */
void boot_ap(uint8_t id)
{
	id &= 0xF;
        uint64_t *apb_idt_ptr = APB_BASE + 0x8;
        *apb_idt_ptr = &idt_ptr;

	uint64_t *apb_apic_setup = APB_BASE + 0x10;
	*apb_apic_setup = &apic_ap_setup;

	volatile uint32_t *ap_count_ptr = (uint32_t *)(APB_BASE + 0x4);
	*ap_count_ptr = system_info->active_cpus;

	lapic_write(apic_ICR_32_63, id << 24);
	lapic_write(apic_ICR_0_31, 0x00004500);
	/* Set up PIT */
	outb(0x61, (inb(0x61) & 0xFD) | 1);
	outb(0x43,0xB2);
	//1193180/100 Hz = 11931 = 2e9bh
	outb(0x42,0x9B);	//LSB
	inb(0x60);		//short delay
	outb(0x42,0x2E);	//MSB

	//reset PIT one-shot counter (start counting)
	uint8_t tmp = inb(0x61)&0xFE;
	outb(0x61,(uint8_t)tmp);		//gate low
	outb(0x61,(uint8_t)tmp|1);	//gate high

	while(!(inb(0x61)&0x20));

	lapic_write(apic_ICR_0_31, 0x00004600 | (APB_BASE/0x1000));
	lapic_write(apic_ICR_32_63, id << 24);

	outb(0x42,0x9B);	//LSB
	inb(0x60);		//short delay
	outb(0x42,0x2E);	//MSB

	//reset PIT one-shot counter (start counting)
	tmp = inb(0x61)&0xFE;
	outb(0x61,(uint8_t)tmp);		//gate low
	outb(0x61,(uint8_t)tmp|1);	//gate high

	while(!(inb(0x61)&0x20));
	outb(0x42,0x9B);	//LSB
	inb(0x60);		//short delay
	outb(0x42,0x2E);	//MSB

	//reset PIT one-shot counter (start counting)
	tmp = inb(0x61)&0xFE;
	outb(0x61,(uint8_t)tmp);		//gate low
	outb(0x61,(uint8_t)tmp|1);	//gate high

	while(!(inb(0x61)&0x20));
	while(*ap_count_ptr == system_info->active_cpus);


		system_info->active_cpus++;
		printf("[SMP]: AP %d booted! Currently %d active processors running.\n", id, *ap_count_ptr);

}
