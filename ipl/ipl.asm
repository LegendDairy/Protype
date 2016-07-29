;Pro-Type Kernel v1.3 ;
;IPL v0.25            ;
;by LegendMythe       ;

[BITS	16]
[ORG 0x500]

jmp main

%include "gdt.inc"
%include "a20.inc"
%include "floppy.inc"
%include "fat12.inc"
%include "print.inc"
%include "e820.inc"

%define kernel_buffer      0x10000     	; Load kernel at 1mb
%define PML4T              0x10000      ; Location of PML4 table
%define image_buffer       0x10000      ; 256KB for file
%define module_buffer      0x50000      ; 192KB for modules
%define image_seg          0x1000       ; Segment (es)
%define module_seg         0x5000       ; segment for module buffer
%define BytesPerSector     512          ; Floppy=512
%define memorymap          0x20000
%define mmap_seg           0x2000

main:
  xor ax, ax                  		; Erase ax
  mov ds, ax                  		; Set up the segments
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov sp, 0xFFF0			; Stack from 0x7E00-0xFFF0

  mov si, MsgIPL              		; IPL Boot Message
  call Print                  		; Print Message

  call a20_kb                 		; Enable A20

  mov ax, mmap_seg			; Setup BIOS Memorymap
  mov es, ax
  xor di, di
  call do_e820				; e820 memorymep
  xor ax, ax
  mov es, ax

  call memsize				; Get total ram
  mov WORD [low_mem], ax		; Store in the ipl struct
  mov WORD [high_mem], bx

  mov si, APB				; Load Aplication processor boot code
  mov ax, module_seg			; Load the APB module
  xor bx, bx
  call LoadFile				; Load module
  mov word [ramdisksize], cx

  mov si, KRNL                        	; Load filename
  mov ax, image_seg                   	; Buffer segment
  xor bx, bx                          	; Buffer offset
  call LoadFile                       	; Load kernel module
  mov word [FileSize], cx

  cli                                 	; Dissable interupts
  lgdt [gdt_ptr]                      	; Load the GDT
  sti                                 	; Enable interupts

  cli
  mov eax, cr0                        	; Read cr0
  or eax, 1                           	; Set PM bit
  mov cr0, eax                        	; Write cr0
  jmp dword 0x8:pm                      ; Far jump in PM

[BITS 32]
pm:
  mov ax, 0x10                        	; Set data descriptors
  mov ds, ax                          	;
  mov es, ax                          	;
  mov fs, ax                          	;

  mov gs, ax
  mov ss, ax                          	; Set up Stack descriptor

  mov eax, DWORD [0x10000 + 0x20]     	; e_phoff
  mov ebp, DWORD [0x10000 + 0x18]     	; e_entry
  xor ecx, ecx
  mov cx, WORD [0x10000 + 0x38]       	; e_phnum
  xor edx, edx
  mov dx, WORD [0x10000 + 0x36]       	; e_phentsize
  add eax, 0x10000                    	; Set base

load_kernel:
  .loop:                              	; Loop through headers
    push ecx
    mov esi, [eax + 0x00]             	; p_type
    mov edi, 0x00001                  	; PT_LOAD: Loadable segment
    std                               	; Increment esi and edi
    cmpsd                               ; Check for PT_LOAD
    jne .skip                         	; Not loadable?=>Not interested!

    mov esi, [eax + 0x08]             	; p_offset
    add esi, 0x10000                  	; address of section
    mov edi, [eax + 0x10]             	; p_vaddr
    mov ecx, [eax + 0x20]             	; p_filesz
    cld
    rep movsb                         	; Copy section in memory

  .skip:
    pop ecx                           	; Reload counter
    add eax, edx                      	; Address of next header
    loop .loop                        	; Loop

jump_long_mode:
  mov ecx, 0x1800                     	; Size to erase /4
  mov eax, PML4T                      	; Base of paging structs
  mov cr3, eax                        	; Set PML4T_ptr in cr3

  .l1:
  mov DWORD [eax], 0x00               	; Erase Page directories
  add eax, 4                          	; Per DWORD
  loop .l1                            	; 4*0x1800=>0x6000

  mov DWORD [PML4T], 0x11003		; PML4T[0]   = &PD_ptr[0] | flags
  mov DWORD [PML4T + 0xFF8], 0x10003	; PML4T[511] = &PML4T     | flags
  mov DWORD [PML4T + 0x1000], 0x12003	; PDT[0]     = &PD_ptr[0] | flags
  mov DWORD [PML4T + 0x1018], 0x15003	; PDT[3]     = &PD_ptr[3] | flags

  mov DWORD [0x12000], 0x16003        	; PD0[0]   = &PT_ptr[0]
  mov DWORD [0x15FF8], 0x10003       	; Virtual addresses of the PDPTs

  mov ecx, 512
  xor eax, eax	                      	; 512 PT entries
  mov eax, 0x00003                      ; Physical address
  mov ebx, 0x16000                      ; Pagetable address

  .l2:                                	; Fill table
  mov DWORD [ebx], eax                	; Physical address
  mov DWORD [ebx + 4], 0              	; Physical address
  add eax, 0x1000                     	; Next page
  add ebx, 8                          	; 8 byte entries
  loop .l2                            	; loop till full

  mov eax, cr4                        	; Load cr4
  or eax, 1 << 5                      	; Set PAE-bit
  mov cr4, eax                        	; Enable PAE

  mov ecx, 0xC0000080                 	; Register to read
  rdmsr                               	; Read EFER MSR
  or eax, 1 << 8                      	; Set LM-bit
  wrmsr                               	; Write EFER MSR

  mov eax, cr0                        	; Read current cr0
  or eax, 1 << 31                     	; Set Paging bit
  mov cr0, eax                        	; Enable paging
  lgdt[gdt_64_ptr]                    	; Load 64 bit GDT
  jmp 0x08:longmode                   	; Jump into longmode

[BITS 64]
longmode:
  mov rax, cr0
  and ax, 0xFFFB		       	; Clear coprocessor emulation CR0.EM
  or ax, 0x2			       	; Set coprocessor monitoring  CR0.MP
  mov cr0, rax
  mov rax, cr4
  or ax, 3 << 9		               	; Set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
  mov cr4, rax

  xor rax, rax
  mov rax, [high_mem]
  mov ebx, 64
  mul ebx
  xor rbx, rbx
  mov rbx, [low_mem]
  add rax, rbx
  mov rbx,  1024
  mul rbx
  mov [mem_sz], rax

  mov rdi, IPL_Struct			; Pass IPL_struct_ptr (system ABI V)
  call rbp				; Call the kernel
  cli					; Stop interrupts
  hlt					; Halt cpu
  jmp $					; loop


FileSize:    dw 0
KRNL         db "KERNEL  SYS"
APB          db "APBM    SYS"
MsgIPL       db "[IPL]: Loading kernel module...", 0x0D, 0x0A, 0x00

IPL_Struct:
magic:       dq 0xBEEFC0DEBEEFC0DE	; IPL Magic code
mem_sz:      dq 0			; Memory size in?
low_mem:     dq 0			; Low memory
high_mem:    dq 0			; High memory
mmap_ptr:    dq memorymap		; Pointer to mem-map for pmm
mmap_ent:    dq 0			; Number of mem-map entries
driven       dq 0			; Bootdrive number
drivetype    dq 0			; Bootdrive type
PML4T_PTR    dq PML4T			; Pointer to PML4T
ramdisksize: dq 0			; Size of initial ramdisk
ramdiskptr:  dq 0			; Pointer to intial ramdisk
