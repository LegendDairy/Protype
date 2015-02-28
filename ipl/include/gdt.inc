%ifndef __GDT_INC_67343546FDCC56AAB872_INCLUDED__
%define __GDT_INC_67343546FDCC56AAB872_INCLUDED__

[BITS 16]

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
  
%endif
