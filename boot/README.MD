Pre IPL: Bootsectorcode
=======================
This is the bootcode for the bootsector to be loaded by the BIOS at 0x7c00. The Pre IPL set's up a stack, and searches the floppy (FAT12 formated) for the ipl.
I'm not very good at assembly, so I based my coded on that of "Neon"/"Mike" from www.brokenthorn.com

Notes:
* Yes, this is a reference to the psp kernel, how I started programming, good old times ;)
