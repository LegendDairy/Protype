/* Pro-Type Kernel v0.2		*/
/* ACPI Tables	   v0.1		*/
/* By LegendDairy		*/

#include<acpi.h>
#include<vmm.h>
#include<heap.h>
#include<mutex.h>
#include<thread.h>

/** Finds the Root System Descriptor Table of the ACPI headers.			**/
RSDT_t *find_rsdt(void)
{
	uint8_t *curr = (uint8_t *)0xe0000;
	uint32_t i, j;
	uint8_t check = 0;

	for (i = 0; i < 0x2000; i++)
	{
		/* Test signature. */
		if (!(strncmp("RSD PTR ", (const char *)curr, 8)))
		{
			/* Calculate checksum. */
			for (j = 0; j < sizeof(RSDP_t); j++)
			{
				check += *curr;
				curr++;
			}

			if (check == 0)
			{
				/* Test if entry is already mapped. */
				if(!vmm_test_mapping((uint64_t)(*(uint32_t*)(curr - 4))))
				{
					/* If not: map and allocate the physical address of the RSDT */
					pmm_bset((uint64_t)(*(uint32_t*)(curr - 4)));
					vmm_map_frame((uint64_t)(*(uint32_t*)(curr - 4)), (uint64_t)(*(uint32_t*)(curr - 4)), 0x3);
				}
				/*We found the RSD. */
				return  (RSDT_t *)((uint64_t)(*(uint32_t*)(curr - 4)));
			}
			/* Continue*/
			curr -= 20;
		}
		/* RSD_PTR has to be 0x10 alligned */
		curr = (uint8_t *)(curr + 0x10);
	}

	/* TODO: Search 1kb of the EBDA. */

	/* Didn't find the RSDT: return NULL */
	return (RSDT_t *)NULL;
}

/** Finds a specific ACPI Table Header. Arg1: the rsdt, Arg2: signature.	**/
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
