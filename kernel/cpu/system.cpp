/* Pro-Type Kernel v0.2		*/
/* System Functions v0.2	*/
/* By LegendDairy		*/

#include <system.hpp>
#include <scheduler.hpp>
#include <heap.h>
#include <cpu.hpp>
#include <acpi.h>
#include <idt.h>

uint32_t *system_c::lapic	= 0;
bool system_c::instance_flag 	= 0;
cpu_c *system_c::cpu_list	= 0;
uint32_t system_c::active_cpus 	= 1;
uint32_t system_c::bootstrap	= 0;
uint64_t system_c::bus_freq	= 0;
io_apic_t *system_c::io_apic	= 0;
uint32_t system_c::flags	= 0;
system_c* system_c::system 	= NULL;
uint8_t system_c::irq_map[16];

system_c* system_c::setup()
{
    if(!instance_flag)
    {
	instance_flag = 1;
        system = new system_c();
        return system;
    }
    else
    {
        return system;
    }
}

cpu_c *system_c::get_current_cpu(void)
{
	cpu_c *iterator = cpu_list;
	uint32_t id = apic_get_id();

	while (iterator && iterator->get_id() != id)
		iterator = iterator->next;

	return iterator;
}
cpu_c *system_c::get_cpu_list(void)
{
	return cpu_list;
}

scheduler_c *system_c::get_current_scheduler(void)
{
	cpu_c *curr = get_current_cpu();
	return curr->scheduler;
}

thread_t * system_c::get_current_thread(void)
{
	cpu_c *current_cpu = get_current_cpu();

	if((uint64_t)current_cpu)
	{
		return current_cpu->scheduler->get_current_thread();
	}

	return 0;
}

uint32_t system_c::get_active_cpus(void)
{
	return active_cpus;
}

void system_c::boot_ap(uint8_t id)
{
	id &= 0xF;
        uint64_t *apb_idt_ptr = (uint64_t *)(APB_BASE + 0x8);
        *apb_idt_ptr = (uint64_t)&idt_ptr;

	uint64_t *apb_apic_setup = (uint64_t*)(APB_BASE + 0x10);
	*apb_apic_setup = (uint64_t)&apic_ap_setup;

	volatile uint32_t *ap_count_ptr = (uint32_t *)(APB_BASE + 0x4);
	*ap_count_ptr = active_cpus;

	lapic_write(apic_ICR_32_63, id << 24);
	lapic_write(apic_ICR_0_31, 0x00004500);

	outb(0x61, (inb(0x61) & 0xFD) | 1);
	outb(0x43,0xB2);
	//1193180/100 Hz = 11931 = 2e9bh
	outb(0x42,0x9B);	//LSB
	inb(0x60);		//short delay
	outb(0x42,0x2E);	//MSB

	//reset PIT one-shot counter (start counting)
	uint8_t tmp = inb(0x61)&0xFE;
	outb(0x61,(uint8_t)tmp);		//gate low
	outb(0x61,(uint8_t)tmp|1);	//gate high

	while(!(inb(0x61)&0x20));

	lapic_write(apic_ICR_0_31, 0x00004600 | (APB_BASE/0x1000));
	lapic_write(apic_ICR_32_63, id << 24);

	outb(0x42,0x9B);	//LSB
	inb(0x60);		//short delay
	outb(0x42,0x2E);	//MSB

	//reset PIT one-shot counter (start counting)
	tmp = inb(0x61)&0xFE;
	outb(0x61,(uint8_t)tmp);		//gate low
	outb(0x61,(uint8_t)tmp|1);	//gate high

	while(!(inb(0x61)&0x20));
	outb(0x42,0x9B);	//LSB
	inb(0x60);		//short delay
	outb(0x42,0x2E);	//MSB

	//reset PIT one-shot counter (start counting)
	tmp = inb(0x61)&0xFE;
	outb(0x61,(uint8_t)tmp);		//gate low
	outb(0x61,(uint8_t)tmp|1);	//gate high

	while(!(inb(0x61)&0x20));
	while(*ap_count_ptr == active_cpus);


	active_cpus++;
	printf("[SMP]: AP %d booted! Currently %d active processors running.\n", id, *ap_count_ptr);
	lapic_write(apic_reg_task_priority, 0x00);			// Accept all interrupts
}


system_c::system_c(void)
{
	/* Parse ACPI information for the MADT header. */
	madt_t *madt = (madt_t *)find_acpi_header((RSDT_t *)find_rsdt(), "APIC");

	/* If find_acpi_header reurns NULL the header was not found. */
	if(!madt)
	{
		printf("[ACPI]: Failed locating MADT table!\n");
	}

	/* Initialise some variables. */
	uint32_t i 		= sizeof(madt_t);
	madt_entry_t *curr 	= (madt_entry_t *)((uint64_t)madt + sizeof(madt_t));
	cpu_list 		= 0;
	flags			= madt->flags;
	lapic			= (uint32_t *)((uint64_t)madt->lapic_address);
	vmm_map_frame((uint64_t)lapic, (uint64_t)lapic, 0x03);
	bootstrap		= (uint32_t)(lapic[0x20/4] >> 24);

	/* If PCAT flag is set, a PICs are present and must be dissabled to use ioapic. */
	if(flags & ACPI_MADT_FLAG_PCAT_COMPAT)
	{
		outb(0x22, 0x70);   	// Select IMCR
		outb(0x23, 1);		// Dissable
	}

	/* Parse the MADT table and store the information in system_info structure. */
	while (i < madt->Length)
	{
		if (curr->entry_type == ACPI_MADT_PROC)
		{
			madt_proc_t *tmp = (madt_proc_t *)curr;

			/* Test if this cpu is not dissabled and ready to boot. */
			if(tmp->flags)
			{
				/* Initialise a new entry for the cpu structure in system_info. */
				cpu_c *cpu_entry = new cpu_c(tmp->apic_id, tmp->proc_id, tmp->flags, bootstrap);

				/* Iterate through the cpu list to find the last entry. */
				if((uint64_t)cpu_list)
				{
					cpu_c *iterator = cpu_list;
					while(iterator->next)
						iterator = iterator->next;
					iterator->next = cpu_entry;
				}
				else
				{
					cpu_list = cpu_entry;
				}

			}
		}
		else if (curr->entry_type == ACPI_MADT_IOAP)
		{
			madt_ioap_t *tmp 		= (madt_ioap_t *)curr;

			/* Initialise a new entry for this io_apic. */
			io_apic_t *io_apic_entry 	= (io_apic_t *)malloc(sizeof(io_apic_t));
			io_apic_entry->next 		= 0;
			io_apic_entry->id 		= tmp->ioap_id;
			io_apic_entry->address		= (uint32_t *)((uint64_t)tmp->address);
			io_apic_entry->int_base		= tmp->int_base;

			if(io_apic)
			{
				/* Itterate through all the io_apic entries to find the end of the list. */
				io_apic_t *iterator = io_apic;
				while(iterator->next)
					iterator=iterator->next;

				/* Add new entry at the end of the list. */
				iterator->next = io_apic_entry;
			}
			else
			{
				/* system_info doesn't contain any ioapic entries yet. */
				io_apic = io_apic_entry;
			}
		}
		else if (curr->entry_type == ACPI_MADT_OVERRIDE)
		{
			madt_overide_t *tmp = (madt_overide_t *)curr;
			/* Add this irq override to the system_info irq map. */
			irq_map[tmp->irq] = tmp->interrupt;
		}

		i    += curr->length;
		curr  = (madt_entry_t *)((uint64_t)curr + (uint32_t)curr->length);
	}

	setup_apic();

	cpu_c *iterator = cpu_list;
	while(iterator)
	{
		if(iterator->get_id() != bootstrap)
			boot_ap(iterator->get_id());
		iterator = iterator->next;
	}
}
