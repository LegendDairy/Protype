Symmetric Multi Processing
==========================
Overview
--------
Systems that have more than one 'logical' cpu are called smp-systems. When the system boots one logical cpu is assigned as the bootstrap processor by the BIOS. This cpu will excute the boot code and has to boot the other cpus, called ap (application processor) by sending IPIs.

In order to support SMP system we have to keep in mind that two cpus can try to access critical memory at the same time. In order to prevent race conditions or two cpu trying to access the same resource at the same time we use spinlocks. More advanced implementations of a spinlock is a mutex, this is spinlock that will yield controll to an other thread while it is spinning.

ACPI: MADT
----------
In order to find out important information like how many CPUs are present, the address of the lapic, how many IOAPIC are present and where... The MADT table of the ACPI headers is parsed and the information is stored in a structure of type topolgy_t called `system_info`.
```C
typedef struct
{
	uint8_t bootstrap;
	uint32_t active_cpus;               // Number of active logical cpus.
	uint32_t *lapic_address;	        // Physical address for the APIC.
	uint64_t bus_freq;
	struct io_apic_t *io_apic;	        // Linked list for available IO APICs.
	uint32_t irq_map[16];	            // ISA Overide f.e. pit = irq_map[0]
	uint32_t flags;		                // Flags (not yet used.)
	processor_t *cpu_list;	            // Linked list of Logical CPUs
} topology_t;
```

Local APIC
----------

IO APIC
-------

Inter Processor Interrupts (IPI)
--------------------------------

Booting AP
----------
In order to boot an AP an 'init'-IPI must be send to it's local apic. The CPU then starts to reset itself (flushing tlb etc...). The Intel documents specify that the reset proces never takes more than 10ms. When the reset is completed the CPU is ready to receive a 'start-up' IPI or 'SIPI'. The SIPI contains a starting address where this logical CPU will start executing in 16 bit real-mode. The kernel will load a 'trampoline code' at this address so that this cpu can trampoline into longmode and start excuting threads.
```C
int startup_ap(uint8_t id)
{
int i = cpu_count;    
load_trampoline_code(start);
apic_send_init_ipi(id);
sleep(10);
apic_send_sipi(id, startup);
sleep(10);

/* Did the AP boot? */
if(cpu_count > i)
  return 0;

/* Try again. */
apic_send_sipi(id, startup);
sleep(10);

/* Did the AP boot? */
if(cpu_count > i)
  return 0;

/* Error failed to boot AP! */  
return 1;
}
```
