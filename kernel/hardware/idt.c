/* Pro-Type Kernel v0.2		*/
/* IDT v1.0			*/
/* By LegendDairy		*/

#include <idt.h>
#include <text.h>

/* The IDT */
idt_entry_t 	idt_entry[256];
/* Pointer structure for the CPU */
idt_ptr_t	idt_ptr;
/* Custom ISR handlers. */
idt_handler_t 	interrupt_handlers[256];

/* Sets a gate in the IDT, i.e. installs a handler. */
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

/* Intialises a Long Mode IDT for all the logical CPUs. */
void init_idt(void)
{
	/* Zero all interrupt handlers initially. */
	 memset ((uint8_t*)&interrupt_handlers, 0, sizeof (idt_handler_t) * 256);

	/* Prepare pointer. */
	idt_ptr.limit = sizeof (idt_entry_t) * 256 - 1;
	idt_ptr.base  = (uint64_t) &idt_entry;

	/* Errase all entries. */
	memset ((uint8_t *)&idt_entry, 0, sizeof (idt_entry_t) * 255);

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

	/* Set the gates for the ISRs and exceptions and IRQs */
	idt_set_gate ( 0, (uint64_t)isr0 , 0x08, 0xEE);
	idt_set_gate ( 1, (uint64_t)isr1 , 0x08, 0xEE);
	idt_set_gate ( 2, (uint64_t)isr2 , 0x08, 0xEE);
	idt_set_gate ( 3, (uint64_t)isr3 , 0x08, 0xEE);
	idt_set_gate ( 4, (uint64_t)isr4 , 0x08, 0xEE);
	idt_set_gate ( 5, (uint64_t)isr5 , 0x08, 0xEE);
	idt_set_gate ( 6, (uint64_t)isr6 , 0x08, 0xEE);
	idt_set_gate ( 7, (uint64_t)isr7 , 0x08, 0xEE);
	idt_set_gate ( 8, (uint64_t)isr8 , 0x08, 0xEE);
	idt_set_gate ( 9, (uint64_t)isr9 , 0x08, 0xEE);
	idt_set_gate (10, (uint64_t)isr10, 0x08, 0xEE);
	idt_set_gate (11, (uint64_t)isr11, 0x08, 0xEE);
	idt_set_gate (12, (uint64_t)isr12, 0x08, 0xEE);
	idt_set_gate (13, (uint64_t)isr13, 0x08, 0xEE);
	idt_set_gate (14, (uint64_t)isr14, 0x08, 0xEE);
	idt_set_gate (15, (uint64_t)isr15, 0x08, 0xEE);
	idt_set_gate (16, (uint64_t)isr16, 0x08, 0xEE);
	idt_set_gate (17, (uint64_t)isr17, 0x08, 0xEE);
	idt_set_gate (18, (uint64_t)isr18, 0x08, 0xEE);
	idt_set_gate (19, (uint64_t)isr19, 0x08, 0xEE);
	idt_set_gate (20, (uint64_t)isr20, 0x08, 0xEE);
	idt_set_gate (21, (uint64_t)isr21, 0x08, 0xEE);
	idt_set_gate (22, (uint64_t)isr22, 0x08, 0xEE);
	idt_set_gate (23, (uint64_t)isr23, 0x08, 0xEE);
	idt_set_gate (24, (uint64_t)isr24, 0x08, 0xEE);
	idt_set_gate (25, (uint64_t)isr25, 0x08, 0xEE);
	idt_set_gate (26, (uint64_t)isr26, 0x08, 0xEE);
	idt_set_gate (27, (uint64_t)isr27, 0x08, 0xEE);
	idt_set_gate (28, (uint64_t)isr28, 0x08, 0xEE);
	idt_set_gate (29, (uint64_t)isr29, 0x08, 0xEE);
	idt_set_gate (30, (uint64_t)isr30, 0x08, 0xEE);
	idt_set_gate (31, (uint64_t)isr31, 0x08, 0xEE);
	idt_set_gate (32, (uint64_t)apic_timer, 0x08, 0xEE);
	idt_set_gate (33, (uint64_t)yield, 0x08, 0xEE);
	idt_set_gate (0x30, (uint64_t)pit_routine, 0x08, 0xEE);
	idt_set_gate (0x3F, (uint64_t)apic_spurious, 0x08, 0xEE);

	/* Flush the old Real mode IDT */
	flush_idt((uint64_t) &idt_ptr);
}

/* Sets a gate in the IDT, i.e. installs a handler. */
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
{
	idt_entry[num].baselow  = (uint16_t)(base & 0xFFFF);
	idt_entry[num].basemid  = (uint16_t)((base >> 16) & 0xFFFF);
	idt_entry[num].basehigh = (uint32_t)(base >> 32) & 0xFFFFFFFF;

	idt_entry[num].selector     = sel;
	idt_entry[num].res = 0;
	idt_entry[num].zero = 0;

	idt_entry[num].flags   = flags;
}

/* The different Exception types. */
const char *exception_messages[] =
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

/* Common Exception handler. */
void isr_handler(regs_t * regs)
{
	if ( interrupt_handlers[regs->int_no] != 0)
	{
		interrupt_handlers[regs->int_no](regs);
	}
	else
	{
		//DebugClearScreen();
		putch('\n');
		DebugPuts(exception_messages[regs->int_no]);
		DebugPuts(": Errorcode: ");
		DebugPutHex(regs->err_code);
		printf(" ,rip: %x", regs->rip);
		DebugPuts(" RAX: ");
		DebugPutHex(regs->rax);
		DebugPuts(" RBX: ");
		DebugPutHex(regs->rbx);
		DebugPuts(" RCX: ");
		DebugPutHex(regs->rcx);
		DebugPuts(" RDX: ");
		DebugPutHex(regs->rdx);
		DebugPuts(" RSP: ");
		DebugPutHex(regs->rsp);
		DebugPuts(" RBP: ");
		DebugPutHex(regs->rbp);
		DebugPuts(" ,RDI: ");
		DebugPutHex(regs->rdi);
		DebugPuts(" ,RSI: ");
		DebugPutHex(regs->rsi);
		if(regs-int_no == 0xe)
		{
			uint64_t faulting_address;
			asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
			DebugPuts(" ,Faulting address: ");
			DebugPutHex(faulting_address);
		}

		for (;;);
	}
}
