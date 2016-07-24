/* Pro-Type Kernel v1.3		*/
/* IDT	v1.0			*/
/* By LegendMythe		*/

#ifndef IDT_H
#define IDT_H

#include <common.h>
#include <stdio.h>

// Structure for an idt entry
typedef struct
{
	uint16_t baselow;
	uint16_t selector;
	uint8_t res;
	uint8_t flags;
	uint16_t basemid;
	uint32_t basehigh;
	uint32_t zero;
}__attribute__((packed)) idt_entry_t;

// Pointer structure for lidt
typedef struct
{
	uint16_t limit;
	uint64_t base;
} __attribute__ ((packed)) idt_ptr_t;

// Structure containing register values when the CPU was interrupted.
typedef struct
{
	uint64_t rsp, rbp, rsi, rdi, rdx, rcx, rbx, rax;
	uint64_t int_no, err_code;
	uint64_t rip, cs, rflags, userrsp, ss;				// Pushed by the processor automatically.
} regs_t;

typedef void (*idt_handler_t)(regs_t *);

void init_idt(void);
void isr_install_handler(idt_handler_t fn, int no);

/* Reference to interrupt.s */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void apic_timer();
extern void pit_routine();
extern void apic_spurious();

#endif
