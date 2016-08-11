/* Pro-Type Kernel v1.3	*/
/* ACPI Tables	   v0.1	*/
/* By LegendMythe	*/

#include<acpi.h>
#include<vmm.h>
#include<heap.h>
#include<mutex.h>



topology_t *system_info;

RSDT_t *find_rsdt(void)
{
	uint8_t *curr = (uint8_t *)0xe0000;
	uint32_t i, j;
	uint8_t check = 0;

	for (i = 0; i < 0x2000; i++)
	{
		if (!(strncmp("RSD PTR ", (const char *)curr, 8)))
		{
			for (j = 0; j < sizeof(RSDP_t); j++)
			{
				check += *curr;
				curr++;
			}

			if (check == 0)
			{
				pmm_bset((uint64_t)(*(uint32_t*)(curr - 4)));
				vmm_map_frame((uint64_t)(*(uint32_t*)(curr - 4)), (uint64_t)(*(uint32_t*)(curr - 4)), 0x3);
				return  (RSDT_t *)((uint64_t)(*(uint32_t*)(curr - 4)));
			}
			curr -= 20;
		}
		curr = (uint8_t *)(curr + 0x10); // RSD_PTR has to be 0x10 alligned
	}

	/* TODO: Search 1kb of the EBDA. */

	return NULL;
}

ACPISDTHeader_t *find_acpi_header(RSDT_t *root, const char *signature)
{
	if(!root)
	{
		return NULL;
	}

	uint32_t i = 0;

	for (i = 0; i < (root->Length - sizeof(ACPISDTHeader_t))/4; i++)
	{
		ACPISDTHeader_t *curr = (ACPISDTHeader_t *)((uint64_t)((uint32_t)root->PointerToOtherSDT[i]));

		if(!vmm_test_mapping((uint64_t)root->PointerToOtherSDT[i]))
		{
			pmm_bset((uint64_t)root->PointerToOtherSDT[i]);
			vmm_map_frame((uint64_t)root->PointerToOtherSDT[i], (uint64_t)root->PointerToOtherSDT[i], 0x3);
		}

		if (!strncmp(signature, (const char *)curr, 4))
		{
			uint8_t check 	= 0;
			uint8_t *tmp	= (uint8_t *)curr;
			uint32_t j 	= 0;

			for (j = 0; j < curr->Length; j++)
			{
				if(!vmm_test_mapping((uint64_t)tmp))
				{
					pmm_bset((uint64_t)tmp);
					vmm_map_frame((uint64_t)tmp, (uint64_t)tmp, 0x3);
				}
				check += *tmp;
				tmp++;
			}

			if(!check)
			{
				return (ACPISDTHeader_t*)curr;
			}
		}
	}

	return NULL;
}

void parse_madt(void)
{
	/* Parse ACPI information for the MADT header. */
	madt_t *madt = (madt_t *)find_acpi_header((RSDT_t *)find_rsdt(), "APIC");

	/* Initialise system_info structure. */
	system_info 			= malloc(sizeof(topology_t));
	system_info->cpu_list 		= 0;
	system_info->io_apic 		= 0;
	system_info->active_cpus 	= 1;

	/* If find_acpi_header reurns NULL the header was not found. */
	if(!madt)
	{
		printf("[ACPI]: Failed locating MADT table!\n");
	}

	/* Initialise some variables. */
	uint32_t i 			= sizeof(madt_t);
	madt_entry_t *curr 		= (madt_entry_t *)((uint64_t)madt + sizeof(madt_t));
	system_info->flags		= madt->flags;
	system_info->lapic_address 	= (uint32_t *)madt->lapic_address;

	/* If PCAT flag is set, a PICs are present and must be dissabled to use ioapic. */
	if(system_info->flags & ACPI_MADT_FLAG_PCAT_COMPAT)
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
				processor_t *cpu_entry 				= malloc(sizeof(processor_t));
				cpu_entry->proc_id 				= tmp->proc_id;
				cpu_entry->apic_id 				= tmp->apic_id;
				cpu_entry->current_thread			= 0;
				cpu_entry->next 				= 0;
				cpu_entry->timer_current_tick			= 0;
				/* Iterate through the cpu list to find the last entry. */
				processor_t *itterator 	= (processor_t *)system_info->cpu_list;
				if(itterator)
				{
					while(itterator->next)
					{
						itterator=itterator->next;
					}
					itterator->next = cpu_entry;

				}
				else
				{
					/* system_info doesn't contain any cpu entries yet. */
					system_info->cpu_list = cpu_entry;
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
			io_apic_entry->address		= (uint32_t *)tmp->address;
			io_apic_entry->int_base		= tmp->int_base;

			if(system_info->io_apic)
			{
				/* Itterate through all the io_apic entries to find the end of the list. */
				io_apic_t *itterator = system_info->io_apic;
				while(itterator->next)
				{
					itterator=itterator->next;
				}
				/* Add new entry at the end of the list. */
				itterator->next = io_apic_entry;
			}
			else
			{
				/* system_info doesn't contain any ioapic entries yet. */
				system_info->io_apic = io_apic_entry;
			}
		}
		else if (curr->entry_type == ACPI_MADT_OVERRIDE)
		{
			madt_overide_t *tmp = (madt_overide_t *)curr;
			/* Add this irq override to the system_info irq map. */
			system_info->irq_map[tmp->irq] = tmp->interrupt;
		}

		i    += curr->length;
		curr  = (madt_entry_t *)((uint64_t)curr + (uint32_t)curr->length);
	}
}

/* TODO: move to seperate file system_info.c */
processor_t *system_info_get_current_cpu(void)
{
	processor_t *current_cpu = system_info->cpu_list;
	while(current_cpu && !(current_cpu->apic_id == lapic_read(apic_reg_id)))
	{
		current_cpu = current_cpu->next;
	}

	return current_cpu;
}

uint32_t *system_info_get_lapic_base(void)
{
	return system_info->lapic_address;
}

uint32_t *system_info_get_ioapic_base(uint8_t id)
{
	io_apic_t *itterator = system_info->io_apic;
	while( itterator && itterator->id != id)
	{
		itterator = itterator->next;
	}
	if(itterator)
	{
		return itterator->address;
	}
	else
	{
		return 0;
	}
}
