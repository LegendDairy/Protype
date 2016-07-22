/* Pro-Type Kernel v1.3	*/
/* Advanced PIC	   v0.2	*/
/* By LegendMythe	*/

#include<apic.h>

volatile uint32_t tick;
uint32_t apic_base;
void pit_sleep(uint32_t millis);

processor_t current_cpu;
processor_list_t processors;

uint8_t inb(uint16_t port)
{
    uint8_t byte;
    asm volatile("inb %1, %0":"=a"(byte): "dN" (port));
    return byte;
}

uint8_t apic_check(void)
{
	uint32_t eax, edx;
	asm volatile("cpuid":"=a"(eax),"=d"(edx):"a"(1):"ecx","ebx");
	return edx & CPUID_FLAG_APIC;
}

void apic_timer(void)
{

}



extern void parse_madt(void);

uint32_t lapic_read(uint32_t r)
{
	return ((uint32_t)(current_cpu.lapic_base[r / 4]));
}

void lapic_write(uint32_t r, uint32_t val)
{
	current_cpu.lapic_base[r / 4] = (uint32_t)val;
}

void setup_apic(void)
{
	mutex_unlock(&processors.lock);
	mutex_unlock(&current_cpu.lock);
	processors.prev = 0;
	processors.current = &current_cpu;
	processors.next = 0;

		/* Fill CPU form */
		mutex_lock(&current_cpu.lock);
		asm volatile ("rdmsr": "=a"((uint32_t *)current_cpu.lapic_base) : "c"(apic_base_msr));
		current_cpu.lapic_base = (uint32_t *)(0xfffff000 & (uint64_t)current_cpu.lapic_base);
		apic_base = (uint64_t)current_cpu.lapic_base;
		current_cpu.id = lapic_read(apic_reg_id);
		current_cpu.flags = CPU_FLAG_BOOTSTRAP;
		mutex_unlock(&current_cpu.lock);

		/* Give information to the user */
		printf("[APIC]: Found Local apic at %x, ID: %d, Version: %x\n", (uint64_t)current_cpu.lapic_base, current_cpu.id, lapic_read(apic_reg_version));

		/* Set up Local APIC */
		printf("[APIC]: Enabling APIC...");
		lapic_write(apic_reg_task_priority, 0x00);			// Accept all interrupts
		lapic_write(apic_lvt_timer_reg, 0x10000);			// Disable timer interrupts
		lapic_write(apic_lvt_thermal_reg, 0x10000);			// Dissable Thermal monitor
		lapic_write(apic_lvt_perf_reg, 0x10000);			// Disable performance counter interrupts
		lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
		lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
		lapic_write(apic_lvt_error_reg, 0x10000);			// Disable error interrupts
		lapic_write(apic_reg_spur_int_vect, 0x00131);			// Enable the APIC and set spurious vector to 48
		lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
		lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
		lapic_write(apic_reg_eoi, 0x00);				// Make sure no interrupts are left
		printf("		Done\n");

		/* Set up IO APIC */
		uint32_t *ioapic_reg 	= (uint32_t*)0xfec00000;
		uint32_t *ioapic_io 	= (uint32_t*)0xfec00010;

		*(uint32_t*)ioapic_reg = (uint32_t)0x12;
		*ioapic_io = (uint32_t)0x30 ;
		*(uint32_t*)ioapic_reg = (uint32_t)0x13;
		*ioapic_io = (uint32_t)0x00;

		parse_madt();

		/* Set up LAPIC Timer*/
		setup_lapic_timer();

}

void setup_lapic_timer(void)
{
	/* Set LAPIC timer as reg int 32 */
	lapic_write(apic_lvt_timer_reg, 0x00030);				// int 32
	lapic_write(apic_div_conf, 0x01);					// Divide by 4

	/* Setup LAPIC Counter */

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

	lapic_write(apic_init_count, 0xFFFFFFFF);


	while(!(inb(0x61)&0x20));

	/* Calculate divisor */
	lapic_write(apic_lvt_timer_reg, 0x10030);
	uint32_t freq = lapic_read(apic_cur_count);
	freq = 0xFFFFFFFF - freq;
	freq = freq*100*4;

	/* Give information to the user */
	printf("[APIC]: Bus frequency:  %dMHz\n", freq / 1000000);

	/* Setup intial count */
	lapic_write(apic_init_count, 10000);
	lapic_write(apic_lvt_timer_reg, (uint32_t)(0x30 | apic_timer_period));
}
