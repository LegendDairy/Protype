/* Pro-Type Kernel v0.2		*/
/* System Functions v0.2	*/
/* By LegendDairy		*/

#ifndef SYSTEM_H
#define SYSTEM_H

#include <scheduler.hpp>
#include <cpu.hpp>
#include <acpi.h>

#define APB_BASE 0x50000

class system_c
{
private:
	/* System Information data */
	static cpu_c *cpu_list;
	static bool instance_flag;
	static uint32_t active_cpus;
	static uint32_t bootstrap;
	static uint64_t bus_freq;
	static io_apic_t *io_apic;					// Linked list for available IO APICs.
	static uint8_t irq_map[16];					// ISA Overide f.e. pit = irq_map[0]
	static uint32_t flags;						// Flags (not yet used.)

	/* Class singleton data & constructor*/
	static system_c *system;
	system_c(void);

public:
	static uint32_t *lapic;
	static uint32_t lapic_read(uint32_t r)
	{
		return ((uint32_t)(lapic[r / 4]));
	}
	static void lapic_write(uint32_t r, uint32_t val)
	{
		lapic[r / 4] = (uint32_t)val;
	}
	static system_c *setup(void);
	static uint32_t get_active_cpus(void);
	static cpu_c *get_current_cpu(void);
	static scheduler_c *get_current_scheduler(void);
	static void boot_ap(uint8_t id);
	static thread_t *get_current_thread(void);
	static cpu_c *get_cpu_list(void);
	static io_apic_t *get_ioapic_list()
	{
		return io_apic;
	}
	static uint64_t get_bus_freq(void)
	{
		return bus_freq;
	}
	static void set_bus_freq(uint64_t freq)
	{
		bus_freq = freq;
	}
};

#endif
