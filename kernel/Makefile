# Pro-Type Kernel v1.3	#
# Makefile		#
# by LegendMythe	#

SOURCES=\
main.o video/text.o cpu/apic.o cpu/acpi.o cpu/mutex.o hardware/idt.o hardware/interrupts.o memory/string.o\

CC = x86_64-elf-gcc
CFLAGS=\
-nostdlib -lgcc -ffreestanding -mno-red-zone -fno-exceptions -O2 -Wall -Wextra\
-I./memory/include -I./cpu/include -I./video/include -I./include/stdinc -I./include -I./hardware/include\

AS = nasm

LDFLAGS=-T link.ld -z
ASFLAGS=-f elf64

all: $(SOURCES) link


link:
	x86_64-elf-gcc -T link.ld -o ~/Protype/bin/kernel.sys -ffreestanding -O2 -nostdlib -z max-page-size=0x1000 $(SOURCES) -lgcc

clean:
	-rm $(SOURCES) 
