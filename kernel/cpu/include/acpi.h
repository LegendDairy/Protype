/* Pro-Type Kernel v1.3	*/
/* ACPI Table	   v0.1	*/
/* By LegendMythe	*/

#include<common.h>
#include<string.h>


#define ACPI_MADT_PROC						0
#define ACPI_MADT_IOAP						1
#define ACPI_MADT_OVERRIDE					2
#define ACPI_MADT_NMI						3
#define ACPI_MADT_LAPIC_NMI					4
#define ACPI_MADT_LAPIC_ADDR_OVERRIDE				5
#define ACPI_MADT_IO_SAPIC					6

typedef struct
{
	char Signature[8];
	uint8_t Checksum;
	char OEMID[6];
	uint8_t Revision;
	uint32_t RsdtAddress;
} __attribute__((packed)) RSDPDescriptor_t;

typedef struct
{
	char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
}__attribute__((packed))  ACPISDTHeader_t;

typedef struct 
{
	char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
	uint32_t PointerToOtherSDT[];
}  __attribute__((packed)) RSDT_t;

typedef struct
{
	uint8_t entry_type;
	uint8_t length;
} madt_entry_t;

typedef struct
{
	uint8_t entry_type;
	uint8_t length;
	uint8_t proc_id;
	uint8_t apic_id;
	uint32_t flags;
}  __attribute__((packed)) madt_proc_t;

typedef struct
{
	uint8_t entry_type;
	uint8_t length;
	uint8_t ioap_id;
	uint8_t res;
	uint32_t address;
	uint32_t int_base;
}  __attribute__((packed)) madt_ioap_t;

typedef struct
{
	uint8_t entry_type;
	uint8_t length;
	uint8_t bus;			// Constqnt 0 (ISA)
	uint8_t irq;
	uint32_t interrupt;
	uint16_t flags;
}  __attribute__((packed)) madt_overide_t;
