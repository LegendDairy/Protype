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
  mov ax, 0x0450                      ; 0x4500=>FAT and root
  mov es, ax                          ; Set segment
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
  xor bx, bx                          ; Copy root dir above bootcode
  call ReadFloppy                     ; Read Root Directory
  mov cx, WORD [bpbRootEntries]       ; Set up loop counter
  mov ax, 0x450
  mov es, ax
  mov di, 0                     ; Locate first root entry en ram
  
.loop:
  pop si                              ; Load Filename
  push si                             ; Save for later
  push cx                             ; Save counter for now 0x640
  mov cx, 0xB                         ; Character counter
  push di                             ; Save current entry
  rep cmpsb                           ; Compare strings
  pop di                              ; Load entry
  je LoadFAT                          ; Equal?
  pop cx                              ; Nope load counter
  add di, 0x20                        ; Next entry
  loop .loop                          ; Try again
  mov ax, 0x1                         ; E1: File not found
  mov si, msgError
  call Print
  hlt
  jmp $
  
LoadFAT:
<<<<<<< HEAD
  add sp, 4			      ; Fix the stack
=======
  pop cx                              ; Fix stack 0x65d
  pop si                              ; Fix stack (maybe try add sp, 4?)
>>>>>>> origin/master
  add di, 0x001A
  mov dx, [es:di]			      ; First cluster
  mov WORD [cluster], dx              ; Save for later
  xor ax, ax                          ; Erase ax
  mov al, BYTE [bpbNumberOfFATS]      ; Number of FATs
  mul WORD [bpbSectorsPerFAT]         ; Sectors used by FATs
  mov cx, ax                          ; Amount of sectors to load
  mov ax, WORD [bpbReservedSectors]   ; Adjust for bootsector
  xor bx, bx                          ; Copy FAT above bootcode
  call ReadFloppy                     ; Load the FATs
  pop bx                              ; Load buffer offset
  pop ax                              ; Load buffer segment 0x684
  mov ax, 0x1000                          ; Set the right segment
  xor bx, bx
  mov es, ax
  xor cx, cx
  push cx

LOAD_IMAGE:                            
  mov ax, WORD [cluster]              ; Load clusternumber
  call ClusterLBA                     ; Convert Cluster to address
  xor cx, cx                          ; Erase ax
  mov cl, BYTE [bpbSectorsPerCluster] ; Amount of sectors
  call ReadFloppy                     ; Load first sector of file
  pop cx
  inc cx
  push cx
  mov ax, WORD [cluster]              ; Load clusternumber
  mov dx, ax                          ; Load clusternumber
  mov cx, ax                          ; Load clusternumber
  shr dx, 0x0001                      ; Divide by two (fast)
  add cx, dx                          ; Add for 3/2
  mov di, cx				;0x6aa
  push bx
  mov bx, 0x4500
  add bx, di
  mov dx, WORD [bx]		              ; Read two bytes from FAT
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
  pop cx                              ; File size in sectors
  xor ax, ax                          ; No errors
  ret                                 ; return(0);


msgError db "Error", 0
%endif
