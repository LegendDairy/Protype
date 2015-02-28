%ifndef __FLOPPY_INC_67343546FDCC56AAB872_INCLUDED__
%define __FLOPPY_INC_67343546FDCC56AAB872_INCLUDED__

[BITS 16]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Converts an LBA to CHS format              ;
;Results stored in datastructure            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LBACHS:                                     ;
  xor dx, dx                                ;
  div WORD [bpbSectorsPerTrack]             ;
  inc dl                                    ;
  mov BYTE [Sector],dl                      ;
  xor dx, dx                                ;
  div WORD [bpbHeadsPerCylinder]            ;
  mov [Head], dl                            ;
  mov [Cylinder], al                        ;
  ret                                       ;
                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Read a number of sectors from Floppy0      ;
;ax=LBA                                     ;
;cx=#sectors                                ;
;ES:bx Buffer to write to                   ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ReadFloppy:                                 ;
  .Main:                                     ;
    mov di, 0x0005                          ;
  .ReadSector:                               ;
    push ax                                 ;
    push bx                                 ;
    push cx                                 ;
    call LBACHS                             ; Convert the LBA
    mov ah, 0x02                            ; Select Biosfunction
    mov al, 1                               ; Number of sectors to read
    mov ch, BYTE [Cylinder]                 ;
    mov cl, BYTE [Sector]                   ;
    mov dh, BYTE [Head]                     ;
    mov dl, BYTE [ebrDriveNumber]           ;
    int 0x13                                ; Invoke BIOS
    jnc .Success                            ; Test for read error
    xor ax, ax                              ; BIOS reset disk
    int 0x13                                ; Invoke BIOS
    dec di                                  ;
    pop cx                                  ;
    pop bx                                  ;
    pop ax                                  ;
    jnz .ReadSector                         ; Try again
    int     0x18                            ;
                                            ;
  .Success:                                 ;
    add bl, BYTE [bpbBytesPerSector]        ;                                ;
    pop cx                                  ;
    pop bx                                  ;
    pop ax                                  ;
    inc ax                                  ;
    add bx, WORD [bpbBytesPerSector]        ; Queue next buffer
    loop .ReadSector                        ;
    ret                                     ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%endif
