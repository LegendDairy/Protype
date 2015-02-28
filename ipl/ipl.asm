[BITS	16]
[ORG 0x500]

jmp main

%include "gdt.inc"
%include "a20.inc" ;0x51f
%include "floppy.inc"		
<<<<<<< HEAD
%include "fat12.inc"	
%include "print.inc"
=======
%include "fat12.inc" ;0x604		
%include "print.inc" ;0x6cb
>>>>>>> origin/master

%define kernel_buffer    0x100000     ; Load kernel at 1mb
%define image_buffer     0x10000      ; 448 KiB for file
%define image_seg        0x1000       ; Segment (es)
%define BytesPerSector   512          ; Floppy=512

main:
<<<<<<< HEAD
  xor ax, ax                          ; Erase ax
  mov ds, ax                          ; Set up the segments
  mov es, ax
  mov fs, ax
  mov gs, ax				             
  mov ss, ax                          
  mov sp, 0xFFFF		                  ; Stack from 0x7E00-0xFFFF      		

  mov si, MsgIPL                      ; IPL Message
=======
xor ax, ax                            ; Erase ax
mov ds, ax
mov es, ax
mov fs, ax
mov ax, 0x0000				                ; Set the stack 0xF000-0xFFFF.
mov ss, ax                            ;
mov sp, 0xFFFF		      		;

mov si, MsgIPL                        ; IPL Message
>>>>>>> origin/master
  call Print                          ; Print Message
  
  call a20_kb                         ; Enable A20
  
  mov si, KRNL                        ; Load filename
  mov ax, image_seg                   ; Buffer segment
  xor bx, bx                          ; Buffer offset
  call LoadFile                       ; Load kernel module
<<<<<<< HEAD
  mov word [FileSize], cx	      ; 0x703                 
=======
  mov word [FileSize], cx                  ; 0x720
>>>>>>> origin/master
  
  cli                                 ; Dissable interupts
  pusha
  lgdt [gdt_ptr]                      ; Load the GDT
  sti                                 ; Enable interupts
  popa

  cli
  mov eax, cr0                        ; Read cr0
<<<<<<< HEAD
  or eax, 1                           ; Set PM bit
  mov cr0, eax                        ; Write cr0 0x714s
  jmp 0x8:pm                          ; Far jump in PM 0x721
  
[BITS 32]  
pm:
  mov ax, 0x10                        ; Set data descriptors 0x720
  mov ds, ax                          ; 
  mov es, ax                          ; 
  mov fs, ax                          ;
  mov gs, ax
  mov	ss, ax                          ; Set up Stack descriptor
  
  mov eax, DWORD [0x10000 + 0x1C]           ; e_phoff
  mov ebx, DWORD [0x10000 + 0x18]           ; e_entry
  xor ecx, ecx
  mov cx, WORD [0x10000 + 0x2C]           ; e_phnum
  xor edx, edx
  mov dx, WORD [0x10000 + 0x2A]           ; e_phentsize
  add eax, 0x10000                    ; Set base
  ;push ecx                            ; Arg of main

load_kernel:  
  .loop:                               ; Loop through headers
    push ecx
    mov esi, [eax + 0x00]
    mov edi, 0x00001
    std
    cmpsd				; 0x754
    jne .skip
  
    mov esi, [eax + 0x04]
    add esi, 0x10000
    mov edi, [eax + 0x0c]
    mov ecx, [eax + 0x10]
    cld
    ;mov ecx, [eax + 0x14]             ; Kernelsz in mem                       ; Argument of main
    rep movsd
  
  .skip:
    pop ecx
    dec ecx ;0x774
    add eax, edx 
    loop .loop
  
call_kernel:    
  mov ebp, 0x100000
  call ebp
  cli
  hlt
  jmp $
  
  
  
  
  
	
  ;xor eax, eax                        ;
  ;mov	ax, word [FileSize]             ;
  ;movzx	ebx, word [bpbBytesPerSector] ;
  ;mul	ebx                             ;
  ;mov	ebx, 4                          ;
  ;div	ebx                             ; Size in dwords
                                      ; Set up the counter
  ;mov esi, image_buffer               ; Source
  ;mov edi, kernel_buffer              ; Destination
  ;cld                                 ; Clear direction flag
  ;mov ecx, eax                        ; Set up counter
  ;rep movsd                           ; Repeat til ecx=0
  
  ;mov eax, [kernel_buffer + 60]       ; e_lfanew
  ;add eax, kernel_buffer              ; Address of File Header
=======
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
>>>>>>> origin/master
  ;Check sig?                         ; Check PE00 sig
  ;add eax, 24                         ; Optional header
  ;add eax, 16                         ; Address of Entrypoint
  ;mov ebp, dword [eax]                ; For debugging
  ;add eax, 12                         ; ImageBase
  ;mov ebx, dword [eax]                ; Address = imagebase+entry
  ;add ebp, ebx                        ; Calculate
  ;call ebp                            ; Start kernel module
  
<<<<<<< HEAD
  ;cli                                 ; Halt the system
  ;hlt
  ;jmp $  
=======
  cli                                 ; Halt the system
  hlt
jmp $  
>>>>>>> origin/master
  
  
FileSize:   dw 0
KRNL        db "KERNEL  SYS"  
MsgIPL 	    db "[IPL]: Loading kernel module...", 0x0D, 0x0A, 0x00

