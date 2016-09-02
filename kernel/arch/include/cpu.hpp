/* Pro-Type Kernel v0.2		*/
/* System Functions v0.2	*/
/* By LegendDairy		*/

#ifndef CPU_HPP
#define CPU_HPP

#include <scheduler.hpp>
#include <stdint.h>

class cpu_c
{
private:
	/** Per CPU information. **/
	uint32_t id;		// Logical ID (APIC ID)
	uint32_t numa_dom;	// NUMA Domain
	uint32_t chip_id;	// Chip/package
	uint32_t core_id;	// Physical core
	uint32_t flags;		// Flags from the MADT

	uint64_t ipi_code;
	uint64_t ipi_data;
	bool ipi_processed;
	/** intialises the data for the IPI.						**/
	void setup_ipi( uint64_t code, uint64_t data);

	static volatile uint32_t *apic;
public:
	/** Constructor: Initialises the data for this logical CPU class. 		**/
	cpu_c(uint8_t apic_id, uint32_t proc_id, uint32_t f, uint32_t bootstrap, volatile uint32_t *apic_va);
	/** Returns the ID of this logical CPU.						**/
	uint32_t get_id(void);
	/** Returns pointer to cpu class of the one excecuting this function.		**/
	static cpu_c *get_current_cpu(void);
	/** Returns the apic ID of the CPU excecuting this function.			**/
	static uint32_t get_current_lapic_id(void);

	/** Sends an IPI to a physical destination and waits till it is received.	**/
	//void send_ipi_physical_sync(uint32_t dest, uint64_t code, uint64_t data);
	/** Sends an IPI to a physical destination.					**/
	//void send_ipi_physical(uint32_t dest, uint64_t code, uint64_t data);
	/** Sends an IPI to all logical CPUs. 						**/
	//void broadcast_ipi(uint8_t dest, uint64_t code, uint64_t data, bool inc_self);

	/** Pointer to the next logical CPU in this linked list.			**/
	cpu_c *next;
	/** Scheduler unit of this logical CPU. 					**/
	scheduler_c *scheduler;
};

#define IPI_CODE_TLB_FLUSH		0x1
#define IPI_CODE_VA_FLUSH		0x2
#define IPI_CODE_ADD_THREAD		0x3
#define IPI_CODE_REMOVE_THREAD		0x4

#endif
