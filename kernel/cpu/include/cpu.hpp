/* Pro-Type Kernel v0.2		*/
/* System Functions v0.2	*/
/* By LegendDairy		*/

#ifndef CPU_HPP
#define CPU_HPP

#include <scheduler.hpp>
#include <stdint.h>
#include <heap.h>

class cpu_c
{
private:
	uint32_t id;		// Logical ID
	uint32_t numa_dom;	// NUMA Domain
	uint32_t chip_id;	// Chip/package
	uint32_t core_id;	// Physical core
	uint32_t flags;		// Flags from the MADT
public:
	cpu_c(uint8_t apic_id, uint32_t proc_id, uint32_t f, uint32_t bootstrap)
	{
		id 		= apic_id;
		core_id 	= proc_id;
		numa_dom 	= 0;
		chip_id 	= 0;
		flags		= f;
		next		= 0;
		scheduler	= new scheduler_c(apic_id, bootstrap );
	}
	uint32_t get_id(void)
	{
		return id;
	}
	cpu_c *next;
	scheduler_c *scheduler;
};



#endif