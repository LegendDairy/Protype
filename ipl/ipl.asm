[BITS	16]
[ORG 0x500]

jmp main

%include "gdt.inc"
%include "a20.inc" ;0x51f
%include "floppy.inc"		
%include "fat12.inc" ;0x604		
%include "print.inc" ;0x6cb

%define kernel_buffer    0x100000     ; Load kernel at 1mb
%define image_buffer     0x10000      ; 448 KiB for file
%define image_seg        0x1000       ; Segment (es)
%define BytesPerSector   512          ; Floppy=512

main:
xor ax, ax                            ; Erase ax
mov ds, ax
mov es, ax
mov fs, ax
mov ax, 0x0000				                ; Set the stack 0xF000-0xFFFF.
mov ss, ax                            ;
mov sp, 0xFFFF		      		;

mov si, MsgIPL                        ; IPL Message
  call Print                          ; Print Message
  
  call a20_kb                         ; Enable A20
  
  mov si, KRNL                        ; Load filename
  mov ax, image_seg                   ; Buffer segment
  xor bx, bx                          ; Buffer offset
  call LoadFile                       ; Load kernel module
  mov word [FileSize], cx                  ; 0x720
  
  cli                                 ; Dissable interupts
  pusha
  lgdt [gdt_ptr]                      ; Load the GDT
  sti                                 ; Enable interupts
  popa

  cli
  mov eax, cr0                        ; Read cr0
  or eax, 1                           ; Set PM bit 0x731
  mov cr0, eax                        ; Write cr0
  jmp 0x8:pm                          ; Far jump in PM 
  
[BITS 32]  
pm:
  mov ax, 0x10                        ; Set data descriptors 0x73D
  mov ds, ax                          ; 
  mov es, ax                          ; 
  ;mov fs, ax                         ;
  mov	ss, ax                          ; Set up Stack descriptor
	
	mov	esp, 90000h                     ; New stack at 0x90000
  xor eax, eax                        ;
  mov	ax, word [FileSize]             ;
  movzx	ebx, word [bpbBytesPerSector] ;
  mul	ebx                             ;
  mov	ebx, 4                          ;
  div	ebx                             ; Size in dwords
                                      ; Set up the counter
  mov esi, image_buffer               ; Source
  mov edi, kernel_buffer              ; Destination
  cld                                 ; Clear direction flag
  mov ecx, eax
  rep movsd                           ; Repeat til ecx=0
  
  mov eax, [kernel_buffer + 60]       ; e_lfanew
  add eax, kernel_buffer              ; Address of File Header
  ;Check sig?                         ; Check PE00 sig
  add eax, 24                         ; Optional header
  add eax, 16                         ; Address of Entrypoint
  mov ebp, dword [eax]                ; For debugging
  add eax, 12                         ; ImageBase
  mov ebx, dword [eax]                ; Address = imagebase+entry
  add ebp, ebx                        ; Calculate
  call ebp                            ; Start kernel module
  
  cli                                 ; Halt the system
  hlt
jmp $  
  
  
FileSize:   dw 0
KRNL        db "KERNEL  SYS"  
MsgIPL 	    db "[IPL]: Loading kernel module...", 0x0D, 0x0A, 0x00

