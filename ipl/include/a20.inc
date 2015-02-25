%ifndef __A20_INC_67343546FDCC56AAB872_INCLUDED__
%define __A20_INC_67343546FDCC56AAB872_INCLUDED__

[BITS 16]

a20_kb:
  cli
  call a20_wait_out
  mov al, 0xAD
  out 0x64, al
  
  call a20_wait_out
  mov al, 0xD0
  out 0x64, al
  
  call a20_wait_in
  in al, 0x60
  or al,2
  push al
  
  call a20_wait_out
  mov al, 0xD1
  out 0x64,al
  
  call a20_wait_out
  pop al
  out 0x60,al
  
  call a20_wait_out
  mov al, 0xAE
  out 0x64, al
  
  call    a20wait_out
  sti
  ret
  
  .a20_wait_out:
    in al, 0x64
    test al, 2
    jnz a20_wait_out
    ret

  .a20_wait_in:
    in al, 0x64
    test al, 1
    jz a20_wait_in
    ret

%endif