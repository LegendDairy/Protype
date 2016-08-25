# Pro-Type Kernel v0.2	#
# Makefile		#
# by LegendMythe	#

SOURCES=\
main.o cpu/system.o cpu/cpu.o vga/text.o vga/printf.o cpu/apic.o cpu/acpi.o tm/mutex.o hardware/idt.o hardware/interrupts.o mm/string.o mm/pmm.o mm/vmm.o mm/heap.o tm/thread.o tm/scheduler.o \

CPP = x86_64-elf-g++
CPPFLAGS =-nostdlib -lgcc -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -fno-exceptions -std=c++11 -O2 -Wall -Wextra -Wno-unused-parameter -mcmodel=large -g \
-I./mm/include -I./cpu/include -I./vga/include -I./include/stdinc -I./include -I./hardware/include -I./tm/include\

CC = x86_64-elf-g++
CFLAGS=\-nostdlib -lgcc -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -fno-exceptions -std=c++11 -O2 -Wall -Wextra -Wno-unused-parameter -mcmodel=large -g\
-I./mm/include -I./cpu/include -I./vga/include -I./include/stdinc -I./include -I./hardware/include -I./tm/include\
#mcmodel=kernel also possible

AS = nasm
ASFLAGS=-f elf64

LDFLAGS=-T link.ld -z -nostdlib -nostartfiles-std=c++11

all: $(SOURCES) link


link:
	x86_64-elf-g++ -T link.ld -o ~/Protype/bin/kernel.sys -ffreestanding -O2 -nostdlib -g -mcmodel=large -z max-page-size=0x1000 $(SOURCES)

clean:
	-rm $(SOURCES)
