/* Pro-Type Kernel v1.3	*/
/* Advanced PIC	   v0.2	*/
/* By LegendMythe		*/

#include<apic.h>

extern u32int tick;
u32int apic_base;
void pit_sleep(u32int millis);

processor_t current_cpu;
processor_list_t processors;


u8int apic_check(void)
{
	u32int eax, edx;
	asm volatile("cpuid":"=a"(eax),"=d"(edx):"a"(1):"ecx","ebx");
	return edx & CPUID_FLAG_APIC;
}

u32int toc;
void apic_timer(void)
{
	DebugPuts("i");
}

extern void parse_madt(void);

u32int lapic_read(u32int r)
{
	return ((u32int)(current_cpu.lapic_base[r / 4]));
}

void lapic_write(u32int r, u32int val)
{
	current_cpu.lapic_base[r / 4] = (u32int)val;
}

void setup_apic(void)
{
	mutex_lock(&processors.lock);
	processors.prev = 0;
	processors.current = &current_cpu;
	processors.next = 0;
	mutex_unlock(&processors.lock);

	if (!apic_check)
	{
		DebugPuts("\nERROR: APIC: 0x1: No APIC found!");
	}
	else
	{
		/* Fill CPU form */
		mutex_lock(&current_cpu.lock);
		asm volatile ("rdmsr": "=a"((u32int *)current_cpu.lapic_base) : "c"(apic_base_msr));
		current_cpu.lapic_base = (u32int*)((u32int)0xfffff000 & (u32int)current_cpu.lapic_base);
		apic_base = current_cpu.lapic_base;
		current_cpu.id = lapic_read(apic_reg_id);
		current_cpu.flags = CPU_FLAG_BOOTSTRAP;
		mutex_unlock(&current_cpu.lock);

		/* Give information to the user */
		DebugPuts("[APIC]: Found Local apic at ");
		DebugPutHex(current_cpu.lapic_base);
		DebugPuts(", ID: ");
		DebugPutDec(current_cpu.id);
		DebugPuts(", Version: ");
		DebugPutDec(lapic_read(apic_reg_version));
		DebugPuts("\n");

		/* Set up Local APIC */
		DebugPuts("[APIC]: Enabling APIC...");
		lapic_write(apic_reg_task_priority, 0x00);			// Accept all interrupts
		lapic_write(apic_lvt_timer_reg, 0x10000);			// Disable timer interrupts
		lapic_write(apic_lvt_thermal_reg, 0x10000);		// Dissable Thermal monitor
		lapic_write(apic_lvt_perf_reg, 0x10000);			// Disable performance counter interrupts
		lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
		lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
		lapic_write(apic_lvt_error_reg, 0x10000);			// Disable error interrupts
		lapic_write(apic_reg_spur_int_vect, 0x00131);			// Enable the APIC and set spurious vector to 48
		lapic_write(apic_lvt_lint0_reg, 0x08700);			// Enable normal external interrupts
		lapic_write(apic_lvt_lint1_reg, 0x00400);			// Enable normal NMI processing
		lapic_write(apic_reg_eoi, 0x00);						// Make sure no interrupts are left
		DebugPuts("		Done\n");

		/* Set up IO APIC */
		u32int *ioapic_reg 	= (u32int*)0xfec00000;
		u32int *ioapic_io 	= (u32int*)0xfec00010;

		*(u32int*)ioapic_reg = (u32int)0x12;
		*ioapic_io = (u32int)0x30 ;
		*(u32int*)ioapic_reg = (u32int)0x13;
		*ioapic_io = (u32int)0x00;
	
		//parse_madt();

		/* Set up LAPIC Timer*/
		setup_lapic_timer();
	}
}

void setup_lapic_timer(void)
{
	/* Set LAPIC timer as reg int 32 */
	lapic_write(apic_lvt_timer_reg, 0x00030);			// int 18
	lapic_write(apic_div_conf, 0x01);					// Divide by 1

	/* Setup LAPIC Counter */
	lapic_write(apic_init_count, 0xFFFFFFFF);

	/* Set up PIT */
	//pit_sleep(1);

	/* Calculate divisor */
	lapic_write(apic_lvt_timer_reg, 0x10030);
	u32int freq = lapic_read(apic_cur_count);
	freq = 0xFFFFFFFF - freq;
	freq = freq*4;

	/* Give information to the user */
	DebugPuts("[APIC]: Bus frequency: ");
	DebugPutDec(freq / 1000);
	DebugPuts(" %dMHz\n");

	/* Setup intial count */
	lapic_write(apic_init_count, 10000);
	lapic_write(apic_lvt_timer_reg, (u32int)(0x30 | apic_timer_period));
}
