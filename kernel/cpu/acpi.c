/* Pro-Type Kernel v1.3	*/
/* ACPI Table	   v0.1	*/
/* By LegendMythe		*/

#include<acpi.h>

ACPISDTHeader_t *find_rsdt(void)
{
	u8int *curr = (u8int *)0xE0000;

	int i;
	for (i = 0; i < 0x10000; i++)
	{
		if (!(strncmp("RSD PTR ", (u8int *)curr, 8)))
		{
			return  (ACPISDTHeader_t *)(*(u32int*)(curr + 16));
		}
		curr = (u8int *)(curr + 0x10);
	}
}

RSDT_t *findMADT(ACPISDTHeader_t *RootSDT)
{
	ACPISDTHeader_t *curr = (RSDT_t *)(RootSDT);
	int i = 0;
	u32int *tmp = (u32int *)curr;

	for (i = 0; i < 0x5000; i++)
	{

		if (*tmp == (u32int) 0x43495041)
		{
			DebugPuts("[ACPI]: Found APIC Table at: ");
			DebugPutHex(tmp);
			DebugPuts("\n");
			return (RSDT_t*)tmp;
		}
		tmp++;
			
	}
	DebugPuts("[ACPI]: No APIC Table found!");
	// No MADT found
	return NULL;
}

void parse_madt(void)
{
	RSDT_t *mad = findMADT((RSDT_t *)find_rsdt());
	u32int i = 0;
	madt_entry_t *curr = (u32int)((u32int)mad + 44);
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
		
		

		curr = ((u32int)((u32int)curr + (u32int)curr->length));
	}
}
