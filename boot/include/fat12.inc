%ifndef __FAT12_INC_67343546FDCC56AAB872_INCLUDED__
%define __FAT12_INC_67343546FDCC56AAB872_INCLUDED__

[BITS 16]

%include "floppy.inc"		

ClusterLBA:                           ; Converts a clusternumber to a LBA
  sub ax, 0x0002                      ; Zero base cluster number.
  mul BYTE [bpbSectorsPerCluster]     ; Multiply by clustersize.
  add ax, WORD [dataregion]           ; Offset in dataregion of FAT12.
  ret                                 ; Return


; Load file at ax:bx ;
; 11 byte Filename: [si]
LoadFile:
  push ax                             ; Save buffer segment
  push bx                             ; Save buffer offset
  push si                             ; Save Filename
  xor cx, cx                          ; Erase cx
  xor dx, dx                          ; Erase dx
  mov ax, 0x0020                      ; 32 byte directory entry
  mul WORD [bpbRootEntries]           ; Total size of directory
  div WORD [bpbBytesPerSector]        ; Sectors used by directory
  xchg ax, cx                         ; Erase ax
  mov al, BYTE [bpbNumberOfFATS]      ; Number of FATs
  mul WORD [bpbSectorsPerFAT]         ; Sectors used by FATs
  add ax, WORD [bpbReservedSectors]   ; Adjust for bootsector
  mov WORD [dataregion], ax           ; Offset of root directory
  add WORD [dataregion], cx           ; Offset of the dataregion
  mov bx, 0x0200                      ; Copy root dir above bootcode
  call ReadFloppy                     ; Read Root Directory
  mov cx, WORD [bpbRootEntries]       ; Set up loop counter
  mov di, 0x0200                      ; Locate first root entry en ram
  
.loop:
  pop si                              ; Load Filename
  push cx                             ; Save counter for now
  mov cx, 0xB                         ; Character counter
  push si                             ; Save for later
  push di                             ; Save current entry
  rep cmpsb                           ; Compare strings
  pop di                              ; Load entry
  je LoadFAT                          ; Equal?
  pop cx                              ; Nope load counter
  add di, 0x20                        ; Next entry
  loop .loop                          ; Try again
  mov ax, 0x1                         ; E1: File not found
  ret                                 ; return(1);

LoadFAT:
  add sp, 4			      ; Fix the stack
  mov dx, WORD [di + 0x001A]          ; First cluster
  mov WORD [cluster], dx              ; Save for later
  xor ax, ax                          ; Erase ax
  mov al, BYTE [bpbNumberOfFATS]      ; Number of FATs
  mul WORD [bpbSectorsPerFAT]         ; Sectors used by FATs
  mov cx, ax                          ; Amount of sectors to load
  mov ax, WORD [bpbReservedSectors]   ; Adjust for bootsector
  mov bx, 0x0200                      ; Copy FAT above bootcode
  call ReadFloppy                     ; Load the FATs
  pop bx                              ; Load buffer offset
  pop ax                              ; Load buffer segment
  xor bx, bx
  mov ax, 0x50
  mov es, ax                          ; Set the right segment


LOAD_IMAGE:                            
  mov ax, WORD [cluster]              ; Load clusternumber
  call ClusterLBA                     ; Convert Cluster to address
  xor cx, cx                          ; Erase ax
  mov cl, BYTE [bpbSectorsPerCluster] ; Amount of sectors
  call ReadFloppy                     ; Load first sector of file
  mov ax, WORD [cluster]              ; Load clusternumber
  mov dx, ax                          ; Load clusternumber
  mov cx, ax                          ; Load clusternumber
  shr dx, 0x0001                      ; Divide by two (fast)
  add cx, dx                          ; Add for 3/2
  push bx
  mov bx, 0x200                       ; FAT addres
  add bx, cx                          ; Next clusternumber
  mov dx, WORD [bx]                   ; Read two bytes from FAT
  pop bx
  test ax, 0x0001                     ; This word = 16bits
  jnz .ODD_CLUSTER                    ; We only need 12 bits

  .EVEN_CLUSTER:                      ; If even only the low bits
    and dx, 0000111111111111b         ; Take low twelve bits (start at 0=even)
    jmp .DONE                         ; Jump over odd bit code
         
  .ODD_CLUSTER:                       ; If odd only the high 12 bits
    shr dx, 0x0004                    ; Take high twelve bits

.DONE:                                 
  mov WORD [cluster], dx              ; Save real 12bit cluster
  cmp dx, 0x0FF0                      ; End of file?
  jb LOAD_IMAGE                       ; Nope, continue...
  mov ax, 0                           ; No errors
  ret                                 ; return(0);

%endif
