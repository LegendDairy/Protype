;Pro-Type Kernel v1.3   ;
;AP initialisation v0.1 ;
;by LegendMythe         ;

[BITS	16]
[ORG 0x50000]

jmp main

ap_id: db 0                     ; ID number of the AP

main:
mov ax, 0xB800
mov es, ax                      ; Pointer to vram
mov al, 0x48                    ; ASCII for 'H'
mov ah, 0x0F                    ; Color attributes
xor edi, edi                    ; Erase offset
stosw                           ; Print 'H' to top left corner

loop:
jmp loop
