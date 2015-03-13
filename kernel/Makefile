# Pro-Type Kernel v1.3	#
# Makefile		#
# by LegendMythe	#

SOURCES=\
main.o video/text.o cpu/apic.o cpu/acpi.o cpu/lock.o cpu/mutex.o hardware/idt.o hardware/interrupts.o memory/string.o\

CFLAGS=\
-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -m64 -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fomit-frame-pointer\
-I./memory/include -I./cpu/include -I./video/include -I./include/stdinc -I./include -I./hardware/include\

LDFLAGS=-T link.ld -z max-page-size=0x1000 -melf_x86_64
ASFLAGS=-f elf64

all: $(SOURCES) link

clean:
	-rm *.o
	-rm cpu/*.o
	-rm memory/*.o
	-rm video/*.o
	-rm hardware/*.o


link:
	ld $(LDFLAGS) -o ~/Protype/bin/kernel.sys  $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
