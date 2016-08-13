Protype x86_64 Microkernel
==========================
Overview
--------
Protype is an x86_64-based microkernel mostly written in C.
The microkernel has it's own two stage bootloader (i.e. the Pre-IPL and IPL).
The kernel is written in C and x86-assembler.
I will try to add documentation in the DOCS folders for every part of the microkernel to clarify the code and discuss algorithms.

The goal is to write a lightweight and fast kernel and use all the options that the x86_64 platform has to offer.
The code is not meant to be portable at all because that would either overcomplicate things or make the code slow.
The kernel will support Long Mode and SMP, in fact it demands support for Long Mode and features like an IOAPIC.
The target cpu is a 2600k

Future Ideas:
-unmap pages arround the stack to detect stack overruns
-Possibility to allign heap structures



Roadmap:
--------
I try to follow a basic roadmap to implement new features. However sometimes I jump a head and try to implement a proof of concept.
This breaks the tedious routine of research, implementation and testing+bugfixes.

  * 2 stage bootloader                     Done (TODO FAT32 i.o FAT10)
  * Long mode jump                         Done (TODO higherhalf)
  * physical memory manager:               Done & tested
  * virtual memory manager                 Done and test (simple version)
  * Heap:                                  Done & tested
  * Preemptive support                     WIP (work in progress)
  * SMP support                            POC (Proof of concept) with premtive support
  * VFS
  * IPC
  * Usermode Exec loading
  * grapics (module)



Repository Overview
-------------------
Current realease number: r1,3 codenamed Hydrogen

/bin  :   Contains the binaries, i.e. the Floppy image, modules...
/boot :   Contains Pre-IPL code, i.e. stage 1 of the bootloader
/ipl  :   Contains the IPL code, i.e. stage 2 of the bootloader
/kernel:  Contains the actual kernel, mainly in C



Build Instructions
------------------
To build the kernel, you can use the build.sh script, first you'll need to give it permission on Linux:
chmod 777 build.sh
cd /media
sudo mkdir floppy1
sudo chmod 777 /media
Then you can run it, if you copy to Protype folder to your home directory
./build.sh
To test:
bochs -q -f bin/bochsrc.txt

To use git on linux
sudo apt-get install git
git config --global user.name "Username"
git config --global user.email "youremail@domain.com"

git clone https://github.com/LegendDairy/Protype.git
git status

git pull

git commit -a
git push

Tools:
GNU gcc crosscompiler with x86_64-elf as target
NASM Netwide Assembler, that came with my distribution
Bochs with GUI Debugger

I wrote most of the code myself, but I also took some code from the internet, because I was to lazy to write it.
For example the printscreen code.
About me:
My real name is Frederic, and I'm from Belgium
