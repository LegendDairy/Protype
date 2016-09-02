Protype x86_64 Microkernel
==========================
Overview
--------
Protype is an x86_64-based microkernel mostly written in C++. The microkernel has it's own two stage bootloader (i.e. the Pre-IPL and IPL). The kernel is written almost entirely in C++. When i started programming I learned the C language, later on C++ and other OOP languages just seemed 'wrong'. Thats why most of the code is written in a procedural manner and looks more like plain old C. However later on I started to have issues with code readability and overal maintainability, so I started to experiment with C++. Thats why now you'll see some OOP here and there. It sorta looks like C with a bit of C++ mangled in there right now.

The goal is to write a lightweight and fast kernel and use all the options that the x86_64 platform has to offer. The code is not meant to be portable at all because that would either overcomplicate things or make the code slow. I *might* restructure the code later on to make it slightly more portabable, but I doubt that I would ever actually port it to an other platform. The kernel will support Long Mode and SMP, in fact it demands support for Long Mode and features like an IOAPIC. The target cpu is a 2600k, because it was the first CPU I bought myself. However my current testbed is a mobile quadcore i7 from the Sandy Bridge generation.

Roadmap:
--------
I try to follow a basic roadmap to implement new features. However sometimes I jump a head and try to implement a proof of concept.
This breaks the tedious routine of research, implementation and testing+bugfixes. I will also try to add documentation in the DOCS folders for every part of the microkernel to clarify the code and discuss algorithms i used. I will also try to include a doc-file per *.c file to quickly explain what one can expect in that file.

#####Implementation list
| Feature | Status    |
| -------------------------------------------------  | ------ |
| 2 stage bootloader  | Done |
| Long mode jump  |Done  |
| Physical memory manager  | Done |
| Virtual memory manager  | Done |
| Kernel heap  | Done |
| Preemptive support  | Done |
| SMP support  | Done |
| Virtual File System  |  |
| Inter Process Communication  |  |
| Usermode  |  |
| Graphics (POC)  |  |

Repository Overview
-------------------
Current realease number: v0.2.

#####Releases:

| Version | Description                                       |
| ------  | ------------------------------------------------- |
| v0.1    | Intial write, low maintainability.                |
| v0.2    | Complete rewrite, added longmode and smp support. |
| v0.3    | First driver modules.                             |

#####Source map:

| dir | description                                       |
| ------  | ------------------------------------------------- |
| /bin    | Contains the binaries, i.e. the Floppy image, modules...               |
| /boot    | Contains Pre-IPL code, i.e. stage 1 of the bootloader |
| /ipl    | Contains the IPL code, i.e. stage 2 of the bootloader    | 
| /kernel    | Contains the actual kernel, mainly in C++  |

#####Kernel source code map:

| dir | description                                       |
| ------  | ------------------------------------------------- |
| /arch    | Contains architecture dependant code.               |
| /mm    | Contains all memory managment code. |
| /tm    | Contains all the preemptive code / multithreading code.    | 
| /vga    | Contains all vga/video related code.  |

Build Instructions
------------------
To build the kernel, you can use the build.sh script, first you'll need to give it permission on Linux:
* `chmod 777 build.sh`
* `cd /media`
* `sudo mkdir floppy1`
* `sudo chmod 777 /media`

Then you can run it, if you copy to Protype folder to your home directory
* `./build.sh`

To test:
* `bochs -q -f bin/bochsrc.txt`

Tools
-----
GNU gcc crosscompiler with x86_64-elf as target (see tools.txt)
NASM Netwide Assembler, that came with my distribution
Bochs with GUI Debugger

About me
--------
I wrote most of the code myself, but I also took some code from the internet, because I was to lazy to write it.
For example the printscreen code. My real name is Frederic, and I'm from Belgium
