;Pro-Type Kernel v1.3 ;
;IPL v1.1             ;
;by LegendMythe       ;

[BITS	16]
[ORG 0x500]

jmp main

%include "gdt.inc"
%include "a20.inc"
%include "floppy.inc"		
%include "fat12.inc"	
%include "print.inc"

%define kernel_buffer    0x100000     ; Load kernel at 1mb
%define image_buffer     0x10000      ; 448 KiB for file
%define image_seg        0x1000       ; Segment (es)
%define BytesPerSector   512          ; Floppy=512

main:
  xor ax, ax                          ; Erase ax
  mov ds, ax                          ; Set up the segments
  mov es, ax
  mov fs, ax
  mov gs, ax				             
  mov ss, ax                          
  mov sp, 0xFFFF		                  ; Stack from 0x7E00-0xFFFF      		

  mov si, MsgIPL                      ; IPL Message
  call Print                          ; Print Message
  
  call a20_kb                         ; Enable A20
  
  mov si, KRNL                        ; Load filename
  mov ax, image_seg                   ; Buffer segment
  xor bx, bx                          ; Buffer offset
  call LoadFile                       ; Load kernel module
  mov word [FileSize], cx	                 
  
  cli                                 ; Dissable interupts
  lgdt [gdt_ptr]                      ; Load the GDT
  sti                                 ; Enable interupts

  cli
  mov eax, cr0                        ; Read cr0
  or eax, 1                           ; Set PM bit
  mov cr0, eax                        ; Write cr0
  jmp 0x8:pm                          ; Far jump in PM
  
[BITS 32]  
pm:
  mov ax, 0x10                        ; Set data descriptors
  mov ds, ax                          ; 
  mov es, ax                          ; 
  mov fs, ax                          ;

  mov gs, ax
  mov ss, ax                          ; Set up Stack descriptor
  
  mov eax, DWORD [0x10000 + 0x20]     ; e_phoff
  mov ebp, DWORD [0x10000 + 0x18]     ; e_entry
  xor ecx, ecx
  mov cx, WORD [0x10000 + 0x38]       ; e_phnum
  xor edx, edx
  mov dx, WORD [0x10000 + 0x36]       ; e_phentsize
  add eax, 0x10000                    ; Set base
  ;push ecx                           ; Arg of main

load_kernel:  
  .loop:                              ; Loop through headers
    push ecx
    mov esi, [eax + 0x00]             ; p_type
    mov edi, 0x00001                  ; PT_LOAD: Loadable segment
    std                               ; Increment esi and edi
    cmpsd				; 0x754               ; Check for PT_LOAD
    jne .skip                         ; Not loadable?=>Not interested!
  
    mov esi, [eax + 0x08]             ; p_offset
    add esi, 0x10000                  ; address of section
    mov edi, [eax + 0x10]             ; p_vaddr
    mov ecx, [eax + 0x20]             ; p_filesz
    cld
    ;mov ecx, [eax + 0x28]            ; Kernelsz in mem
    rep movsd                         ; Copy section in memory
  
  .skip:
    pop ecx                           ; Reload counter
    add eax, edx                      ; Address of next header
    loop .loop                        ; Loop
  
jump_long_mode:

  mov ecx, 0x1800                     ; Size to erase /4
  mov eax, 0x10000                    ; Base of paging structs
  
  mov cr3, eax                        ; Set PML4T_ptr in cr3
  
  .l1:
  mov DWORD [eax], 0x00               ; Erase Page directories
  add eax, 4                          ; Per DWORD
  loop .l1                            ; 4*0x1800=>0x6000
    
  mov DWORD [0x10000], 0x11003        ; PML4T[0] = &PD_ptr[0]
  mov DWORD [0x11000], 0x12003        ; PDT[0]   = &PD_ptr[0]
  mov DWORD [0x11018], 0x15003        ; PDT[3]   = &PD_ptr[3]
  
  mov DWORD [0x12000], 0x16003        ; PD0[0]   = &PT_ptr[0]
  mov DWORD [0x15FB0], 0x17003        ; PD3[502] = &PT_ptr[502]
  mov DWORD [0x15FB8], 0x18003        ; PD3[503] = &PT_ptr[503]
  
  mov ecx, 512                        ; 512 PT entries
  mov eax, 0x00003                    ; Physical address
  mov ebx, 0x16000                    ; Pagetable address
  
  .l2:                                ; Fill table
  mov DWORD [ebx], eax                ; Physical address
  mov DWORD [ebx + 4], 0              ; Physical address
  add eax, 0x1000                     ; Next page
  add ebx, 8                          ; 8 byte entries
  loop .l2                            ; loop till full
    
  mov ecx, 512                        ; 512 PT entries       
  mov eax, 0xFEC00003                 ; Physical address of IOAPIC
  mov ebx, 0x17000                    ; Pagetable address of table
  
  .l4:                                ; Fill table 
  mov DWORD [ebx], eax                ; Physical address
  mov DWORD [ebx + 4], 0              ; Physical address
  add eax, 0x1000                     ; Next page
  add ebx, 8                          ; 8 byte entries
  loop .l4                            ; loop till full
    
  mov ecx, 512                        ; 512 PT entries
          
  mov eax, 0xFEE00003                 ; Physical address of LAPIC
  mov ebx, 0x18000                    ; Pagetable address of table
    
  .l3:                                ; Fill table 
  mov DWORD [ebx + 4], 0              ; Physical address
  mov DWORD [ebx], eax                ; Physical address
  add eax, 0x1000                     ; Next page
  add ebx, 8                          ; 8 byte entries
  loop .l3                            ; loop till full
    
  mov eax, cr4                        ; Load cr4
  or eax, 1 << 5                      ; Set PAE-bit
  mov cr4, eax                        ; Enable PAE
  

  
  mov ecx, 0xC0000080                 ; Register to read
  rdmsr                               ; Read EFER MSR
  or eax, 1 << 8                      ; Set LM-bit
  wrmsr                               ; Write EFER MSR
    
  mov eax, cr0                        ; Read current cr0
  or eax, 1 << 31                     ; Set Paging bit
  mov cr0, eax                        ; Enable paging
  
  lgdt[gdt_64_ptr]                    ; Load 64 bit GDT
  jmp 0x08:longmode                   ; Jump into longmode
  
[BITS 64]  
longmode:
  mov rbp, 0x100000
  call rbp
  cli
  hlt
  jmp $
  
  
FileSize:   dw 0
KRNL        db "KERNEL  SYS"  
MsgIPL 	    db "[IPL]: Loading kernel module...", 0x0D, 0x0A, 0x00

