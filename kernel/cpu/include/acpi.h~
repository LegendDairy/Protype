/* Pro-Type Kernel v1.3	*/
/* ACPI Table	   v0.1	*/
/* By LegendMythe		*/

#include<common.h>

#define ACPI_MADT_PROC						0
#define ACPI_MADT_IOAP						1
#define ACPI_MADT_OVERRIDE					2
#define ACPI_MADT_NMI						3
#define ACPI_MADT_LAPIC_NMI					4
#define ACPI_MADT_LAPIC_ADDR_OVERRIDE		5
#define ACPI_MADT_IO_SAPIC					6

typedef struct
{
	char Signature[8];
	u8int Checksum;
	char OEMID[6];
	u8int Revision;
	u32int RsdtAddress;
} __attribute__((packed)) RSDPDescriptor_t;

typedef struct
{
	char Signature[4];
	u32int Length;
	u8int Revision;
	u8int Checksum;
	char OEMID[6];
	char OEMTableID[8];
	u32int OEMRevision;
	u32int CreatorID;
	u32int CreatorRevision;
}__attribute__((packed))  ACPISDTHeader_t;

typedef struct 
{
	char Signature[4];
	u32int Length;
	u8int Revision;
	u8int Checksum;
	char OEMID[6];
	char OEMTableID[8];
	u32int OEMRevision;
	u32int CreatorID;
	u32int CreatorRevision;
	u32int PointerToOtherSDT[];
}  __attribute__((packed)) RSDT_t;

typedef struct
{
	u8int entry_type;
	u8int length;
} madt_entry_t;

typedef struct
{
	u8int entry_type;
	u8int length;
	u8int proc_id;
	u8int apic_id;
	u32int flags;
}  __attribute__((packed)) madt_proc_t;

typedef struct
{
	u8int entry_type;
	u8int length;
	u8int ioap_id;
	u8int res;
	u32int address;
	u32int int_base;
}  __attribute__((packed)) madt_ioap_t;

typedef struct
{
	u8int entry_type;
	u8int length;
	u8int bus;			// Constqnt 0 (ISA)
	u8int irq;
	u32int interrupt;
	u16int flags;
}  __attribute__((packed)) madt_overide_t;
