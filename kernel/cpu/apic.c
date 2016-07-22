/* Pro-Type Kernel v1.3	*/
/* Advanced PIC	   v0.2	*/
/* By LegendMythe	*/

#include<apic.h>

volatile uint64_t tick;
uint32_t apic_base;
processor_t current_cpu;
processor_list_t processors;
extern void parse_madt(void);
void pit_sleep(uint32_t millis);

uint8_t apic_check(void)
{
	uint32_t eax, edx;
	asm volatile("cpuid":"=a"(eax),"=d"(edx):"a"(1):"ecx","ebx");
	return edx & CPUID_FLAG_APIC;
}

void apic_timer(void)
{
	tick++;
}

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
	current_cpu.lapic_base &= (uint32_t *)(0xfffff000);
	apic_base = (uint64_t)current_cpu.lapic_base;
	current_cpu.id = lapic_read(apic_reg_id);
	current_cpu.flags = CPU_FLAG_BOOTSTRAP;
	mutex_unlock(&current_cpu.lock);

	/* Set up Local APIC */
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

	/* Set up IO APIC */
	uint32_t *ioapic_reg 	= (uint32_t*)0xfec00000;
	uint32_t *ioapic_io 	= (uint32_t*)0xfec00010;
	*(uint32_t*)ioapic_reg 	= (uint32_t)0x12;
	*ioapic_io 		= (uint32_t)0x30 ;
	*(uint32_t*)ioapic_reg 	= (uint32_t)0x13;
	*ioapic_io 		= (uint32_t)0x00;

	/* Parse the multiprocessor table */
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
	lapic_write(apic_init_count, 0xFFFFFFFF);

	/* Set up PIT */
	uint8_t val;
	asm volatile ( "inb %1, %0" : "=a"(val) : "Nd"(0x61) );
	val &= 0xFD
	val |= 0x1;
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(0x61) );

	asm volatile ( "outb %0, %1" : : "a"(0xb2), "Nd"(0x43) );
	asm volatile ( "outb %0, %1" : : "a"(0X4A9), "Nd"(0x42) );


	//pit_sleep(1);

	/* Calculate divisor */
	lapic_write(apic_lvt_timer_reg, 0x10030);
	uint32_t freq = lapic_read(apic_cur_count);
	freq = 0xFFFFFFFF - freq;
	freq = freq*4;

	/* Give information to the user */
	printf("[APIC]: Bus frequency:  %dMHz\n", freq / 1000);

	/* Setup intial count */
	lapic_write(apic_init_count, 10000);
	lapic_write(apic_lvt_timer_reg, (uint32_t)(0x30 | apic_timer_period));
}

void boot_ap(uint8_t id)
{
	id &= 0xF;
	lapic_write(apic_ICR_32_63, id << 24);
	lapic_write(apic_ICR_0_31, 0x00004500);
}
