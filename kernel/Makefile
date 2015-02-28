# Pro-Type Kernel v1.3	#
# Makefile		#
# by LegendMythe	#

SOURCES=\
main.o\

CFLAGS=\
-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -m32\

LDFLAGS=-T link.ld -melf_i386
ASFLAGS=-f elf

all: $(SOURCES) link


link:
	ld $(LDFLAGS) -o  ~/Protype/bin/kernel.sys $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

# To make a kernel.pe:
# LDFLAGS=-T link.ld -melf_i386 => -melf: emulate 32bit hardware
# link.ld OUTPUT_FORMAT("pei-i386")
