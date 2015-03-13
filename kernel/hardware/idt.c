/* Pro-Type Kernel v1.3		*/
/* IDT v1.0			*/
/* By LegendMythe		*/

#include <idt.h>

idt_entry_t idt_entry[256];
idt_ptr_t	idt_ptr;
idt_handler_t interrupt_handlers[256];

extern void flush_idt(u32int);
void pit_handler(regs_t *r);

void init_idt(void)
{
	/* Zero all interrupt handlers initially. */
	 memset (&interrupt_handlers, 0, sizeof (idt_handler_t) * 256);

	/* Prepare pointer. */
	idt_ptr.limit = sizeof (idt_entry_t) * 256 - 1;
	idt_ptr.base  = (u64int) &idt_entry;

	/* Errase all entries. */
	memset (&idt_entry, 0, sizeof (idt_entry_t) * 255);

	/* Remap the 8259A PIC */
	outb(0x20, 0x11);	// starts the initialization sequence (in cascade mode)
	outb(0xA0, 0x11);	// starts the initialization sequence (in cascade mode)
	outb(0x21, 0x20);	// ICW2: Master PIC vector offset
	outb(0xA1, 0x28);	// ICW2: Slave PIC vector offset
	outb(0x21, 0x04);	// ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(0xA1, 0x02);	// ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(0x21, 0x01);	// ICW4: 80x86 mode
	outb(0xA1, 0x01);	// ICW4: 80x86 mode
	outb(0x21, 0xff);	// Mask all interrupts, ie dissable master
	outb(0xA1, 0xff);	// Mask all interrupts, ie dissable slave

	/*outb(0x22, 0x70);	// SMP Mode old hardware
	outb(0x23, 0x01);	// We should read the ACPI */

	idt_set_gate ( 0, (u64int)isr0 , 0x08, 0xEE);
	idt_set_gate ( 1, (u64int)isr1 , 0x08, 0xEE);
	idt_set_gate ( 2, (u64int)isr2 , 0x08, 0xEE);
	idt_set_gate ( 3, (u64int)isr3 , 0x08, 0xEE);
	idt_set_gate ( 4, (u64int)isr4 , 0x08, 0xEE);
	idt_set_gate ( 5, (u64int)isr5 , 0x08, 0xEE);
	idt_set_gate ( 6, (u64int)isr6 , 0x08, 0xEE);
	idt_set_gate ( 7, (u64int)isr7 , 0x08, 0xEE);
	idt_set_gate ( 8, (u64int)isr8 , 0x08, 0xEE);
	idt_set_gate ( 9, (u64int)isr9 , 0x08, 0xEE);
	idt_set_gate (10, (u64int)isr10, 0x08, 0xEE);
	idt_set_gate (11, (u64int)isr11, 0x08, 0xEE);
	idt_set_gate (12, (u64int)isr12, 0x08, 0xEE);
	idt_set_gate (13, (u64int)isr13, 0x08, 0xEE);
	idt_set_gate (14, (u64int)isr14, 0x08, 0xEE);
	idt_set_gate (15, (u64int)isr15, 0x08, 0xEE);
	idt_set_gate (16, (u64int)isr16, 0x08, 0xEE);
	idt_set_gate (17, (u64int)isr17, 0x08, 0xEE);
	idt_set_gate (18, (u64int)isr18, 0x08, 0xEE);
	idt_set_gate (19, (u64int)isr19, 0x08, 0xEE);
	idt_set_gate (20, (u64int)isr20, 0x08, 0xEE);
	idt_set_gate (21, (u64int)isr21, 0x08, 0xEE);
	idt_set_gate (22, (u64int)isr22, 0x08, 0xEE);
	idt_set_gate (23, (u64int)isr23, 0x08, 0xEE);
	idt_set_gate (24, (u64int)isr24, 0x08, 0xEE);
	idt_set_gate (25, (u64int)isr25, 0x08, 0xEE);
	idt_set_gate (26, (u64int)isr26, 0x08, 0xEE);
	idt_set_gate (27, (u64int)isr27, 0x08, 0xEE);
	idt_set_gate (28, (u64int)isr28, 0x08, 0xEE);
	idt_set_gate (29, (u64int)isr29, 0x08, 0xEE);
	idt_set_gate (30, (u64int)isr30, 0x08, 0xEE);
	idt_set_gate (31, (u64int)isr31, 0x08, 0xEE);
	idt_set_gate (48, (u64int)apic_routine, 0x08, 0xEE);
	idt_set_gate (49, (u64int)apic_spurious, 0x08, 0xEE);

	flush_idt((u64int) &idt_ptr);
}

void idt_set_gate(u8int num, u64int base, u16int sel, u8int flags)
{
	idt_entry[num].baselow  = (u16int)(base & 0xFFFF);
	idt_entry[num].basemid  = (u16int)((base >> 16) & 0xFFFF);
	idt_entry[num].basehigh = (u32int)(base >> 32) & 0xFFFFFFFF;
	
	idt_entry[num].selector     = sel;
	idt_entry[num].res = 0;
	idt_entry[num].zero = 0;
	
	idt_entry[num].flags   = flags;
}

unsigned char *exception_messages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",

	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",

	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void isr_handler(regs_t * regs)
{
	if ( interrupt_handlers[regs->int_no] != 0)
	{ 
		interrupt_handlers[regs->int_no](regs); 
	}
	else
	{
		u32int n = (u32int)regs->int_no;
		DebugPuts("\n[");
		DebugSetTextColour(0x4, 0);
		DebugPuts("ISR");
		DebugSetTextColour(0xF, 0);
		DebugPuts("]");
		DebugPuts(exception_messages[regs->int_no]);
		DebugPuts("\n[");
		DebugSetTextColour(0x4, 0);
		DebugPuts("Kernel");
		DebugSetTextColour(0xF, 0);
		DebugPuts("]\nHalted.\n");

		for (;;);
	}
}

