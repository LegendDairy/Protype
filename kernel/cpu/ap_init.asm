;Pro-Type Kernel v1.3   ;
;AP initialisation v0.1 ;
;by LegendMythe         ;

[BITS	16]
[ORG 0x50000]

jmp main

ap_id:          db 0			; ID number of the AP (image[0x3])
ap_count:       db 1			; Number of active processors (image[0x4])
gdt_ptr         dw 0			; image[0x6])
gdt_ptr		dw 0
PLM4T:          dq 0x1000		; Address of the PLM4T (image[0x8])

main:
xor ax, ax                              ; Erase ax
mov ds, ax                              ; Set up the segments
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
mov sp, 0xFFF0		            	; Stack from 0x7E00-0xFFF0

mov ax, 0xB800
mov es, ax				; Pointer to vram
mov al, 0x48				; ASCII for 'H'
mov ah, 0x0F				; Color attributes
xor edi, edi				; Erase offset
stosw					; Print 'H' to top left corner

loop:
jmp loop
