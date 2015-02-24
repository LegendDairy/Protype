%ifndef __PRINT_INC_67343546FDCC56AAB872_INCLUDED__
%define __PRINT_INC_67343546FDCC56AAB872_INCLUDED__

[BITS 16]

Print:
  lodsb   				                    ; Load next byte from string from SI to AL
  or	al, al			                    ; Does AL=0?
	jz	PrintDone		                    ; Yep, null terminator found-bail out
	mov	ah, 0eh			                    ; Nope-Print the character
	int	10h                             ; Invoke BIOS
	jmp	Print			                      ; Repeat until null terminator found
                                      ;
	PrintDone:                          ;
    ret				                        ; We are done, so return


%endif
