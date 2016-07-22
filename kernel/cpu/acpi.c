/* Pro-Type Kernel v1.3	*/
/* ACPI Tables	   v0.1	*/
/* By LegendMythe	*/

#include<acpi.h>


ACPISDTHeader_t *find_rsdt(void)
{
	uint8_t *curr = (uint8_t *)0xe0000;

	int i, j;
	uint8_t check = 0;

	for (i = 0; i < 0x2000; i++)
	{
		if (!(strncmp("RSD PTR ", (const char *)curr, 8)))
		{
			for (j = 0; j < 20; j++)
			{
				check += *curr;
				curr++;
			}
			if (check == 0)
			{
				for(j=0; j<0x100; j++)
				{
					pmm_bset(((uint64_t)(*(uint32_t*)(curr - 4)) + j*0x1000));
					vmm_map_frame(((uint64_t)(*(uint32_t*)(curr - 4)) + j*0x1000), ((uint64_t)(*(uint32_t*)(curr - 4)) + j*0x1000), 0x3);
				}

				return  (ACPISDTHeader_t *)((uint64_t)(*(uint32_t*)(curr - 4)));
			}

		}
		curr = (uint8_t *)(curr + 0x10); // RSD_PTR has to be 0x10 alligned
	}

return NULL;
}

ACPISDTHeader_t *findMADT(ACPISDTHeader_t *RootSDT)
{
	ACPISDTHeader_t *curr = (ACPISDTHeader_t *)(RootSDT);

	if(!curr)
	{
		return NULL;
	}

	int i = 0;
	uint8_t *tmp = (uint8_t *)curr;

	for (i = 0; i < 0x5000; i++)
	{

		if (!strncmp("APIC", tmp, 4))
		{
			return (ACPISDTHeader_t*)tmp;
		}
		tmp++;

	}
	return NULL;
}

void parse_madt(void)
{
	ACPISDTHeader_t *mad = findMADT((ACPISDTHeader_t *)find_rsdt());

	if(!mad)
	{
		printf("[ACPI]: Failed parsing MADT table!\n");
	}

	uint32_t i = 0;
	madt_entry_t *curr = (madt_entry_t *)((uint64_t)mad + 44);
	while (i < mad->Length - 44)
	{
		if (curr->entry_type == ACPI_MADT_PROC)
		{
			madt_proc_t *tmp = (madt_proc_t *)curr;

			printf("[ACPI] Found processor: %d, apic id: %d, flags: %x\n", tmp->proc_id, tmp->apic_id, tmp->flags);
			i += 8;
		}
		if (curr->entry_type == ACPI_MADT_IOAP)
		{
			madt_ioap_t *tmp = (madt_ioap_t *)curr;
			printf("[ACPI] Found IO APIC ID: %d, address: %x, int base: %x\n", tmp->ioap_id, tmp->address, tmp->int_base);
			i += 12;
		}
		if (curr->entry_type == ACPI_MADT_OVERRIDE)
		{
			madt_overide_t *tmp = (madt_overide_t *)curr;
			printf("[ACPI] Found Interrupt Source Override: IRQ: %d, , Global int: %d\n", tmp->irq, tmp->interrupt);
			i += 10;
		}
		curr = (madt_entry_t *)((uint64_t)curr + (uint32_t)curr->length);
	}
}
