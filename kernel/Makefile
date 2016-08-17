# Pro-Type Kernel v1.3	#
# Makefile		#
# by LegendMythe	#

SOURCES=\
main.o video/text.o video/printf.o cpu/apic.o  cpu/acpi.o cpu/mutex.o hardware/idt.o hardware/interrupts.o memory/string.o memory/pmm.o memory/vmm.o memory/heap.o tm/thread.o tm/scheduler.o\

CC = x86_64-elf-gcc
CFLAGS=\
-nostdlib -lgcc -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -fno-exceptions -O2 -Wall -Wextra   -mcmodel=large\
-I./memory/include -I./cpu/include -I./video/include -I./include/stdinc -I./include -I./hardware/include -I./tm/include\
#mcmodel=kernel also possible
AS = nasm

LDFLAGS=-T link.ld -z
ASFLAGS=-f elf64

all: $(SOURCES) link


link:
	x86_64-elf-gcc -T link.ld -o ~/Protype/bin/kernel.sys -ffreestanding -O2 -nostdlib  -mcmodel=large  -z max-page-size=0x1000 $(SOURCES) -lgcc

clean:
	-rm $(SOURCES)
