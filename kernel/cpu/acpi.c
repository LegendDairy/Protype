/* Pro-Type Kernel v1.3	*/
/* ACPI Table	   v0.1	*/
/* By LegendMythe	*/

#include<acpi.h>

ACPISDTHeader_t *find_rsdt(void)
{
	uint8_t *curr = (uint8_t *)0xE0000;

	int i;
	for (i = 0; i < 0x10000; i++)
	{
		if (!(strncmp("RSD PTR ", (const char *)curr, 8)))
		{
		return  (ACPISDTHeader_t *)((u64int)(*(uint32_t*)(curr + 16)));
		}
		curr = (uint8_t *)(curr + 0x10);
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
			DebugPuts("[ACPI]: Found APIC Table at: ");
			DebugPutHex((uint64_t)tmp);
			DebugPuts("\n");
			return (ACPISDTHeader_t*)tmp;
		}
		tmp++;
			
	}
	DebugPuts("[ACPI]: No APIC Table found!");
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

			DebugPuts("[ACPI] Found processor: ");
			DebugPutDec(tmp->proc_id);
			DebugPuts(", apic id: ");
			DebugPutDec(tmp->apic_id);
			DebugPuts(", flags: ");
			DebugPutDec(tmp->flags);
			DebugPuts("\n");

			i += 8;
		}
		if (curr->entry_type == ACPI_MADT_IOAP)
		{
			madt_ioap_t *tmp = (madt_ioap_t *)curr;
			
			DebugPuts("[ACPI] Found IO APIC ID: ");
			DebugPutDec(tmp->ioap_id);
			DebugPuts(", address: ");
			DebugPutHex(tmp->address);
			DebugPuts(", int base:");
			DebugPutHex(tmp->int_base);
			DebugPuts("\n");

			i += 12;
		}
		if (curr->entry_type == ACPI_MADT_OVERRIDE)
		{
			madt_overide_t *tmp = (madt_overide_t *)curr;

			DebugPuts("[ACPI] Found Interrupt Source Override: IRQ: ");
			DebugPutDec(tmp->irq);
			DebugPuts(", Global int: ");
			DebugPutDec(tmp->interrupt);
			DebugPuts("\n");

			i += 10;
		}
		
		

		curr = (madt_entry_t *)((uint64_t)curr + (uint32_t)curr->length);
	}
}
