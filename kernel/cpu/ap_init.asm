;Pro-Type Kernel v1.3   ;
;AP initialisation v0.1 ;
;by LegendMythe         ;

[BITS	16]
[ORG 0x50000]

jmp main

ap_count: db 0x01
idt_ptr:  dq 0x0

main:
cli
mov al, [ap_count]
inc al
mov [ap_count], al


xor eax, eax
mov ax, ds
shl eax, 4
add eax, gdt_start
mov [gdt_ptr + 2], eax
mov eax, gdt_end
sub eax, gdt_start
mov eax, dword gdt_ptr


cli                                 	; Dissable interupts
lgdt [gdt_ptr]                      	; Load the GDT
sti                                 	; Enable interupts

cli
mov eax, cr0                        	; Read cr0
or eax, 1                           	; Set PM bit
mov cr0, eax                        	; Write cr0
jmp dword 0x8:pm

[BITS 32]
pm:
xor eax, eax
mov ax, 0x10                        	; Set data descriptors
mov ds, ax                          	;
mov es, ax                          	;
mov fs, ax                          	;

mov gs, ax
mov ss, ax                          	; Set up Stack descriptor

xor eax, eax
add eax, gdt_64_start
mov [gdt_64_ptr + 2], qword eax
mov eax, gdt_64_end
sub eax, gdt_64_start
mov [gdt_64_start], ax

mov eax, 0x10000                      ; Base of paging structs
mov cr3, eax                        ; Set PML4T_ptr in cr3

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
mov rax, cr0
and ax, 0xFFFB		       ; Clear coprocessor emulation CR0.EM
or ax, 0x2			       ; Set coprocessor monitoring  CR0.MP
mov cr0, rax
mov rax, cr4
or ax, 3 << 9		               ; Set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
mov cr4, rax

mov rbx, 0xB8000
mov al, 0x48				; ASCII for 'H'
mov ah, 0x0F				; Color attributes
mov [rbx], ax

loop:
jmp loop

gdt_start:
  dd 0                      ; Null descriptor
  dd 0                      ; Null descriptor
                            ; Code Descriptor
  dw 0xFFFF                 ; Limit low
  dw 0                      ; Base low
  db 0                      ; Base mid
  db 10011010b              ; Flags
  db 11001111b              ; Flags + limit
  db 0                      ; Base high
                            ; Data Descriptor
  dw 0xFFFF                 ; Limit low
  dw 0                      ; Base low
  db 0                      ; Base mid
  db 10010010b              ; Acces byte
  db 11001111b              ; Flags + limit
  db 0                      ; Base high

gdt_end:

gdt_ptr:
  dw gdt_end - gdt_start - 1
  dd gdt_start

  gdt_64_start:
    dd 0                      ; Null descriptor
    dd 0                      ; Null descriptor
                              ; Code Descriptor
    dw 0                      ; Limit low
    dw 0                      ; Base low
    db 0                      ; Base mid
    db 10011000b              ; Flags
    db 00100000b              ; Flags + limit
    db 0                      ; Base high
                              ; Data Descriptor
    dq 0x0000920000000000

  gdt_64_end:

  gdt_64_ptr:
    dw gdt_64_end - gdt_64_start - 1
    dq gdt_64_start
