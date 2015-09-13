/* Pro-Type Kernel v1.3	*/
/* ACPI Table	   v0.1	*/
/* By LegendMythe	*/

#include<acpi.h>


ACPISDTHeader_t *find_rsdt(void)
{
	uint8_t *curr = (uint8_t *)0xE0000;

	int i, j;
	uint8_t check = 0;

	for (i = 0; i < 0x2000; i++)
	{
		if (!(strncmp("RSD PTR ", (const char *)curr, 8)))
		{
			for (j = 0; j < 20; i++)
			{
				check += *curr;
				curr++;
			}
			if (check == 0)
			{

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
	int i = 0;
	uint32_t *tmp = (uint32_t *)curr;

	for (i = 0; i < 0x5000; i++)
	{

		if (*tmp == (uint32_t) 0x43495041)
		{
			printf("[ACPI]: Found APIC Table at: %x\n", (uint64_t)tmp);
			return (ACPISDTHeader_t*)tmp;
		}
		tmp++;
			
	}
	printf("[ACPI]: No APIC Table found!\n");
	// No MADT found
	return NULL;
}

void parse_madt(void)
{
	ACPISDTHeader_t *mad = findMADT((ACPISDTHeader_t *)find_rsdt());
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
