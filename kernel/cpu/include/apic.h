/* Pro-Type Kernel v1.3	*/
/* Advanced PIC	   v0.1	*/
/* By LegendMythe		*/

#ifndef APIC_H
#define APIC_H

#include<common.h>


/* Initialises the Local APIC. */
void setup_apic(void);

/* Reads/writes to a register of the Local APIC. */
u32int	apic_read_reg(u32int reg);
void	apic_write_reg(u32int reg, u32int val);

#define CPUID_FLAG_MSR 0x20
#define CPUID_FLAG_APIC 0x200


/* Local APIC register map */
#define apic_id								0x0020		//R/W
#define apic_version						0x0030		//RO
#define	apic_task_priority					0x0080		//R/W
#define apic_arbitration_priority			0x0090		//RO
#define apic_processor_priority				0x00A0		//RO
#define apic_eoi							0x00B0		//WO
#define apic_remote_read					0x00C0		//RO
#define apic_logical_dest					0x00D0		//R/W
#define apic_dest_format					0x00E0		//R/W
#define apic_spur_int_vect					0x00F0		//R/W

/* In-Service Register */
#define apic_ISR_0_31						0x0100		//RO
#define apic_ISR_32_63						0x0110		//RO
#define apic_ISR_64_95						0x0120		//RO
#define apic_ISR_96_127						0x0130		//RO
#define apic_ISR_128_159					0x0140		//RO
#define apic_ISR_160_191					0x0150		//RO
#define apic_ISR_192_223					0x0160		//RO
#define apic_ISR_224_255					0x0170		//RO

/* Trigger Mode Register */
#define apic_TMR _0_31						0x0180		//RO
#define apic_TMR_32_63						0x0190		//RO
#define apic_TMR_64_95						0x01A0		//RO
#define apic_TMR_96_127						0x01B0		//RO
#define apic_TMR_128_159					0x01C0		//RO
#define apic_TMR_160_191					0x01D0		//RO
#define apic_TMR_192_223					0x01E0		//RO
#define apic_TMR_224_255					0x01F0		//RO

/* LAPIC Interrupt Request Register */
#define apic_IRR_0_31						0x0200		//RO
#define apic_IRR_32_63						0x0210		//RO
#define apic_IRR_64_95						0x0220		//RO
#define apic_IRR_96_127						0x0230		//RO
#define apic_IRR_128_159					0x0240		//RO
#define apic_IRR_160_191					0x0250		//RO
#define apic_IRR_192_223					0x0260		//RO
#define apic_IRR_224_255					0x0270		//RO

#define apic_error_status_reg				0x0280		//RO
#define apic_LVT_CMCI_reg					0x02F0		//R/W

/* LAPIC Interrupt Command Register */
#define apic_ICR_0_31						0x0300		//R/W
#define apic_ICR_32_63						0x0310		//R/W

/* LAPIC Local Interrupts */
#define apic_lvt_timer_reg					0x0320		//R/W
#define apic_lvt_thermal_reg					0x0330		//R/W
#define apic_lvt_perf_reg					0x0340		//R/W
#define apic_lvt_lint0_reg					0x0350		//R/W
#define apic_lvt_lint1_reg					0x0360		//R/W
#define apic_lvt_error_reg					0x0370		//R/W

/* LAPIC Timer */
#define apic_init_count						0x0380		//R/W
#define apic_cur_count						0x0390		//RO
#define apic_div_conf						0x03E0		//R/W

#define apic_timer_div_1					11
#define apic_timer_div_2					0x00
#define apic_timer_div_4					0x01
#define apic_timer_div_8					0x02
#define apic_timer_div_16					0x03
#define apic_timer_div_32					0x08
#define apic_timer_div_64					0x09
#define apic_timer_div_128					10
#define apic_timer_one_shot					0x00
#define apic_timer_period					0x20000
#define apic_timer_tsc_dead					0x40000





#define apic_base_msr 0x1B
#define apic_base_msr_bsp 0x100
#define apic_base_msr_enable 0x800

#endif										
