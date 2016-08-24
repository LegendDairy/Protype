/* Pro-Type Kernel v0.2		*/
/* System Functions v0.1	*/
/* By LegendDairy		*/

#ifndef SYSTEM_H
#define SYSTEM_H

#include <scheduler.hpp>
#include <cpu.hpp>
#include <acpi.h>

/** Address where the trampoline code for the APs is stored. 				**/
#define APB_BASE 0x50000

typedef struct io_apic_t
{
	struct io_apic_t *next;		// Pointer to next entry in the list.
	uint32_t volatile *address;	// Physical address of this ioapic
	uint8_t id;			// ID of this IO APIC
	uint32_t int_base;		// ?
} io_apic_t;

/** Static/Singleton class to perform SMP/system functions. 				**/
class system_c
{
private:
	/* System Information data */
	static cpu_c *cpu_list;		// Linked list of all bootable logical CPUs
	static uint32_t active_cpus;	// Number of active logical cpus.
	static uint32_t bootstrap;	// APIC ID of the bootstrap processor.
	static uint64_t bus_freq;	// Frequency of the front side bus.
	static io_apic_t *io_apic;	// Linked list for available IO APICs.
	static uint8_t irq_map[16];	// ISA Overide f.e. pit = irq_map[0]
	static uint32_t flags;		// Flags (not yet used.)

	/** Class singleton data & constructor						**/
	static system_c *system;
	static bool instance_flag;

	/** Singleton: Private Constructor.						**/
	/** Parses the MADT, initialises the BSP APIC, IO APIC and boots APs. 		**/
	system_c(void);

	/** Boots an AP processor.							**/
	static void boot_ap(uint8_t id);

public:
	/** Physical address of memory mapped APIC registers. 				**/
	static uint32_t *lapic;

	/** Initialises the system: Browse MADT and boot APs and setup schedulers. 	**/
	static system_c *setup(void);

	/** Get the number of active CPUs.						**/
	static uint32_t get_active_cpus(void);

	/** Get current working CPU.							**/
	static cpu_c *get_current_cpu(void);

	/** Get current scheduler.							**/
	static scheduler_c *get_current_scheduler(void);

	/** Get current thread structure from the active scheduler.			**/
	static thread_t *get_current_thread(void);

	/** Get the first entry of the CPU linked List	.				**/
	static cpu_c *get_cpu_list(void);

	/** Get the first entry of the ioapic linked list.				**/
	static io_apic_t *get_ioapic_list()
	{
		return io_apic;
	}

	/** Get Bus frequency								**/
	static uint64_t get_bus_freq(void)
	{
		return bus_freq;
	}

	/** Set the bus frequency (setup_lapic_timer). 					**/
	static void set_bus_freq(uint64_t freq)
	{
		bus_freq = freq;
	}

	/*
	static uint32_t lapic_read(uint32_t r)
	{
		return ((uint32_t)(lapic[r / 4]));
	}
	static void lapic_write(uint32_t r, uint32_t val)
	{
		lapic[r / 4] = (uint32_t)val;
	} */
};

#endif
