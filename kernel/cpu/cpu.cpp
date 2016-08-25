/* Pro-Type Kernel v0.2		*/
/* CPU Information Class v0.1	*/
/* By LegendDairy		*/

#include <cpu.hpp>
#include <system.hpp>
#include <stdint.h>
#include <heap.h>

/* intializer for static variable. */
volatile uint32_t  *cpu_c::apic	= 0;

/** Constructor: Initialises the data for this logical CPU class. 		**/
cpu_c::cpu_c(uint8_t apic_id, uint32_t proc_id, uint32_t f, uint32_t bootstrap, volatile uint32_t *apic_va)
{
	id 		= apic_id;
	core_id 	= proc_id;
	numa_dom 	= 0;
	chip_id 	= 0;
	flags		= f;
	next		= 0;
	scheduler	= new scheduler_c(apic_id, bootstrap);
	apic		= apic_va;
	ipi_processed	= 1;
}

/** Returns the ID of this logical CPU.						**/
uint32_t cpu_c::get_id(void)
{
	return id;
}

/** Returns pointer to cpu class of the one excecuting this function.		**/
cpu_c *cpu_c::get_current_cpu(void)
{
	return system::get_cpu_by_id(apic[0x20/4] >> 24);
}

/** Sends an IPI to a physical destination.					**/
void cpu_c::send_ipi_physical(uint32_t dest, uint64_t code, uint64_t data)
{
	cpu_c *cpu = system::get_cpu_by_id(dest);
	dest->setup_ipi(code, data);
	apic[0x310/4] = (dest&0xF) << 24;
	apic[0x300/4] = SIPI_GENERAL_VECTOR | IPI_DEST_NORMAL | IPI_PHYSICAL;
}

/** Sends an IPI to a physical destination and waits till it is received.	**/
void cpu_c::send_ipi_physical_sync(uint32_t dest, uint64_t code, uint64_t data)
{
	cpu_c *cpu = system::get_cpu_by_id(dest);
	cpu->setup_ipi(code, data);
	apic[0x310/4] = (dest&0xF) << 24;
	apic[0x300/4] = SIPI_GENERAL_VECTOR | IPI_DEST_NORMAL | IPI_PHYSICAL;
	while((volatile uint32_t) apic[0x300/4] & (1 << 12));
}

/** intialises the data for the IPI.						**/
void cpu_c::setup_ipi( uint64_t code, uint64_t data)
{
	while(!ipi_processed);
	ipi_code 	= code;
	ipi_data 	= data;
	ipi_processed 	= 0;
}

/** Sends an IPI to all logical CPUs. 						**/
void cpu_c::broadcast_ipi(uint8_t dest, uint64_t code, uint64_t data, bool inc_self)
{
	cpu_c *iterator = cpu_c *system_c::get_cpu_list();

	while (iterator)
	{
		iterator->setup_ipi(code, data);
		iterator = iterator->next;
	}

	apic[0x310/4] = (0xFF) << 24;
	apic[0x300/4] = SIPI_GENERAL_VECTOR | IPI_DEST_NORMAL | IPI_PHYSICAL |  IPI_DEST_BROADCAST;
}

/** Returns the apic ID of the CPU excecuting this function.			**/
uint32_t cpu_c::get_current_lapic_id(void)
{
	return (system_c::lapic[0x20/4] >> 24);
}
