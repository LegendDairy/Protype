/* Pro-Type Kernel v1.3	*/
/* Advanced PIC	   v0.1	*/
/* By LegendMythe		*/

#include<apic.h>

u32int *apic_base;
extern u32int tick;
void pit_sleep(u32int millis);

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

void setup_apic(void)
{

	if (!apic_check)
	{
		DebugPuts("\nERROR: APIC: 0x1: No APIC found!");
	}
	else
	{
		asm volatile ("rdmsr": "=a"((u32int *)apic_base) : "c"(apic_base_msr));
		apic_base = (u32int*)((u32int)0xfffff000 & (u32int)apic_base);

		DebugPuts("[APIC]: Found Local apic at ");
		DebugPutHex((u32int)apic_base);
		DebugPuts(", ID: ");
		DebugPutDec(apic_base[apic_id / 4]);
		DebugPuts("\n");

		DebugPuts("[APIC]: Enabling APIC...");
		apic_base[apic_task_priority/4]		= (u32int)0x00;
		apic_base[apic_lvt_timer_reg/4]		= (u32int)0x10000;		// Disable timer interrupts
		apic_base[apic_lvt_thermal_reg/4]	= (u32int)0x10000;
		apic_base[apic_lvt_perf_reg/4]		= (u32int)0x10000;		// Disable performance counter interrupts
		apic_base[apic_lvt_lint0_reg/4]		= (u32int)0x08700;		// Enable normal external interrupts
		apic_base[apic_lvt_lint1_reg/4]		= (u32int)0x00400;		// Enable normal NMI processing
		apic_base[apic_lvt_error_reg/4]		= (u32int)0x10000;		// Disable error interrupts
		
		

		apic_base[apic_spur_int_vect/4]		= (u32int)0x00131;		// Enable the APIC and set spurious vector to 48
		apic_base[apic_lvt_lint0_reg/4]		= (u32int)0x08700;		// Enable normal external interrupts
		apic_base[apic_lvt_lint1_reg/4]		= (u32int)0x00400;		// Enable normal NMI processing
		apic_base[apic_eoi/4]	= 0;
		DebugPuts("		Done\n");
	}
	
	u32int *ioapic_reg 	= (u32int*)0xfec00000;
	u32int *ioapic_io 	= (u32int*)0xfec00010;


	*(u32int*)ioapic_reg = (u32int)0x12;
	*ioapic_io = (u32int)0x30 ;
	*(u32int*)ioapic_reg = (u32int)0x13;
	*ioapic_io = (u32int)0x00;
	
	//parse_madt();
	//setup_lapic_timer();


}

void setup_lapic_timer(void)
{
	apic_base[apic_lvt_timer_reg/4]	= (u32int)0x00030;			// int 18
	apic_base[apic_div_conf/4]		= 1;						// Divide by 1

	/* Setup APIC Counter*/
	apic_base[apic_init_count/4] = 0xFFFFFFFF;

	//pit_sleep(1);

	/* Calculate divisor*/
	apic_base[apic_lvt_timer_reg/4] = (u32int)0x10030;
	u32int freq = (u32int)apic_base[apic_cur_count/4];
	freq = 0xFFFFFFFF - freq;
	freq = freq*4;

	DebugPuts("[APIC]: Bus frequency: ");
	DebugPutDec(freq / 1000);
	DebugPuts(" %dMHz\n");

	apic_base[apic_init_count/4] = freq;
	apic_base[apic_lvt_timer_reg/4] = (u32int)(0x30 | apic_timer_period);
}
