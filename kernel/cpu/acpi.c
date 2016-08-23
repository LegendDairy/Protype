/* Pro-Type Kernel v0.2		*/
/* ACPI Tables	   v0.1		*/
/* By LegendDairy		*/

#include<acpi.h>
#include<vmm.h>
#include<heap.h>
#include<mutex.h>
#include<thread.h>

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
				if(!vmm_test_mapping((uint64_t)(*(uint32_t*)(curr - 4))))
				{
					pmm_bset((uint64_t)(*(uint32_t*)(curr - 4)));
					vmm_map_frame((uint64_t)(*(uint32_t*)(curr - 4)), (uint64_t)(*(uint32_t*)(curr - 4)), 0x3);
				}
				return  (RSDT_t *)((uint64_t)(*(uint32_t*)(curr - 4)));
			}
			curr -= 20;
		}
		curr = (uint8_t *)(curr + 0x10); // RSD_PTR has to be 0x10 alligned
	}

	/* TODO: Search 1kb of the EBDA. */

	return (RSDT_t *)NULL;
}

ACPISDTHeader_t *find_acpi_header(RSDT_t *root, const char *signature)
{
	if(!root)
	{
		return (ACPISDTHeader_t *)NULL;
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

	return (ACPISDTHeader_t *)NULL;
}
