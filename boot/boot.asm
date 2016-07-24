;Pro-Type Kernel v1.3 ;
;Pre IPL v1.0         ;
;by LegendMythe       ;

[BITS 16]
[ORG 0x00]

jmp loader

;;;;;;;;;;BIOS Parameter Block;;;;;;;;;;;
bpbOEM:                 db "PROTYPE "   ;                                       ; image[0x3]
bpbBytesPerSector:      dw 512          ;
bpbSectorsPerCluster:   db 1            ;
bpbReservedSectors:     dw 1            ;
bpbNumberOfFATS:        db 2            ;
bpbRootEntries:         dw 224          ;
bpbNumberOfSectors:     dw 2880         ;
bpbMediaDescriptor:     db 0xF8         ;
bpbSectorsPerFAT:       dw 9            ;
bpbSectorsPerTrack:     dw 18           ;
bpbHeadsPerCylinder:    dw 2            ;
bpbHiddenSectors:       dd 0            ;
bpbSectorsLarge:        dd 0            ;
;;;;;;;;;;Extended Boot Record;;;;;;;;;;;
ebrDriveNumber          db 0            ;
ebrNTFlags              db 0            ;
ebrSignature            db 0x29         ;
ebrSerialNumber:        dd 0x12345678   ;
ebrVolumeLabel          db "PROTYPE 1  ";
ebrFileSystem:          db "FAT12   "   ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%include "floppy.inc"
%include "fat12.inc"
%include "print.inc"

loader:           		; Loads stage2.
cli				; Disable interrupts.
mov ax, 0x07C0			; Set up segments.
mov ds, ax        		;
mov es, ax        		;
mov fs, ax        		;
mov gs, ax        		;
mov ax, 0x0000			; Set the stack 0xF000-0xFFFF.
mov ss, ax        		;
mov sp, 0xFFFF    		;
sti               		; Restore interrupts.

xor ah, ah
mov al, 0x3
int 0x10

mov si, MsgBoot                 ; Print a boot message.
call Print                      ;

mov ax, 0x0050                  ; Load stage 2 at 0x50:00
xor bx, bx                      ; Erase bx
mov si, Stage2                  ; Argument: Filename
call LoadFile                   ; Load KRNLDR.SYS
or ax, ax                       ; Test return value
jnz failure                     ; If ax=!0: Fail

.DONE:
push WORD 0x0050
push WORD 0x0000
retf

failure:
mov si, MsgError
call Print
hlt

Stage2      db "IPL     SYS"
MsgBoot     db "[Pre IPL]: Loading IPL...", 0x0D, 0x0A, 0x00
MsgError    db 0x0D, 0x0A, "[Pre IPL]: Error 0x0001: Failed to load IPL!", 0x0D, 0x0A, 00
cluster     dw 0
dataregion  dw 0
Cylinder    db 0
Sector      db 0
Head        db 0

times 510 -($-$$) db 0
dw 0xAA55
