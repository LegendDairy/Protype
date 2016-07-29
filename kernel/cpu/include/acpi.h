/* Pro-Type Kernel v1.3	*/
/* ACPI Table	   v0.1	*/
/* By LegendMythe	*/

#include<common.h>
#include<string.h>
#include<stdio.h>
#include<apic.h>


#define ACPI_MADT_PROC						0
#define ACPI_MADT_IOAP						1
#define ACPI_MADT_OVERRIDE					2
#define ACPI_MADT_NMI						3
#define ACPI_MADT_LAPIC_NMI					4
#define ACPI_MADT_LAPIC_ADDR_OVERRIDE				5
#define ACPI_MADT_IO_SAPIC					6
#define ACPI_MADT_LOCAL_SAPIC					7
#define ACPI_MADT_PLATFORM_INTERRUPT_SOURCES			8
#define ACPI_MADT_PROCESSOR_LOCAL_x2APIC			9
#define ACPI_MADT_LOCAL_x2APIC_NMI				0xA
#define ACPI_MADT_GIC						0xB
#define ACPI_MADT_GICD						0xC
#define ACPI_MADT_FLAG_PCAT_COMPAT				0x1

struct processor_t;
struct io_apic_t;

typedef struct processor_t
{
	struct processor_t *next;
	uint32_t proc_id;
	uint32_t apic_id;
	uint32_t flags;
} processor_t;

typedef struct
{
	uint8_t bootstrap;
	uint32_t active_cpus;
	uint32_t *lapic_address;
	struct io_apic_t *io_apic;
	uint32_t irq_map[16];
	uint32_t flags;
	processor_t *cpu_list;
} topology_t;

typedef struct io_apic_t
{
	struct io_apic_t *next;
	uint32_t *address;
	uint32_t id;
	uint32_t int_base;
} io_apic_t;

typedef struct
{
	char Signature[8];
	uint8_t Checksum;
	char OEMID[6];
	uint8_t Revision;
	uint32_t RsdtAddress;
} __attribute__((packed)) RSDP_t;

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
	uint32_t lapic_address;
	uint32_t flags;
}__attribute__((packed))  madt_t;

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
} __attribute__((packed)) madt_entry_t;

/* Multiple APIC Description Table */
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
	uint8_t reserved;
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
