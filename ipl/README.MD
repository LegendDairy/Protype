Initial Program Loader
======================
Overview
--------
The IPL or Intial Program Loader, loads the kernel module (ELF formated).

The IPL is loaded by the Pre-IPL at 0x500.
The IPL uses 0x4500 for loading the root directory and FATs of the FAT12 filesystem, so the IPL can't be bigger than 16KB.
The IPL starts in 16 bit realmode, loads the kernel module: "kernel.sys" at 0x10000.
Therefore the kernel module has to be smaller than 256KB.
The IPL also enables A20 to access all the memory and sets up a GDT and finaly jumps in to protected mode.
Then the IPL copies the kernel image to 0x100000, parses the module, and loads the sections in the memory.
After that the IPL enables PAE, Longmode and paging and does a far jump to Long mode code.
Finaly the entry point is stored in RBP and the IPL calls the kernel

Memory Map
----------
Before far jump(in RM):
* 0x00000500 ->  0x02500:         GDT + IPL Code
* 0x00004500 -> ~0x04FFF:         Scrap memory for Root Directory and FATS
* 0x00005000 ->  0x0FFFF:         Pre IPL and IPL Stack
* 0x00007C00 ->  0x07DFF:         Pre-IPL code
* 0x00007E00 ->  0x9FFFF:         IPL stack (+- 10kb)
* 0x00010000 ->  0x4FFFF:         Kernel Image buffer (256kb) + PLM4T (after loading kernel ELF)
* 0x00050000 ->  0x7FFFF:         Modules (192kb)

After far jump(in LM):
* 0x00000500 -> 0x00002500:       GDT somewhere...
* 0x0000a000 -> 0x0000FFFF:       Kernel Stack ~22k
* 0x00010000 -> 0x00018FFF:       Kernel Page dir and some tables

Changelog
v0.1: Original
v0.2: Support for Long Mode
v0.25: Support for BIOS memory map