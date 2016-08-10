;Pro-Type Kernel v1.3  ;
;Interrupts v1.2       ;
;by LegendMythe        ;

[BITS 64]
%define apic_eoi	0x00B0

[GLOBAL flush_idt]                  ; Allows the C code to call idt_flush().
[EXTERN idt_ptr]
flush_idt:
    mov rax, idt_ptr                ; Get the pointer to the IDT, passed as a parameter.
    lidt [rax]                      ; Load the IDT pointer.
    ret

[GLOBAL isr_common_stub]
[EXTERN isr_handler]
isr_common_stub:
    cli

    push rax                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push rbx                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push rcx                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push rdx                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push rdi                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push rsi                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push rbp                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push rsp                        ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov rdi, rsp

    cld                             ; ABI neeeds cld
    call isr_handler                ; Call into our C code.

    pop rsp                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    pop rbp                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    pop rsi                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    pop rdi                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    pop rdx                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    pop rcx                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    pop rbx                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    pop rax                         ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    add rsp, 16                     ; Cleans up the pushed error code and pushed ISR number
    iretq                           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

[GLOBAL apic_timer]
[EXTERN tm_schedule]
; TODO: Save the SSE MMX etc registers before thread switch. */
[EXTERN apic_base]
apic_timer:
  cli
  push rax                           	; Dissable interrupts

  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  xor rax, rax
  mov ax, ds
  push rax

  mov rax, cr3
  push rax


  mov rdi, rsp
  cld
  call tm_schedule           ; Call C function
  mov rsp, rax

  pop rax
  mov cr3, rax

  xor rax, rax
  pop rax
  mov ds, ax
  mov es, ax
  mov fs, ax

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  xor rax, rax
  mov eax, [apic_base]			; Apic Base in C-code
  mov dword [eax + apic_eoi], 0x00		; Dissable software for this cpu
  pop rax
  iretq                             	; Return to code

[GLOBAL pit_routine]
[EXTERN pit_handler]
[EXTERN apic_base]
pit_routine:
cli
push rax                           	; Dissable interrupts

push rbx
push rcx
push rdx
push rsi
push rdi
push rbp
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

xor rax, rax
mov ax, ds
push rax

mov rax, cr3
push rax


mov rdi, rsp
cld
call pit_handler           ; Call C function
xor rax, rax
mov eax, [apic_base]              ; Apic Base in C-code
mov dword [eax + apic_eoi],  0    ; Send EOI to LAPIC

pop rax
mov cr3, rax

xor rax, rax
pop rax
mov ds, ax
mov es, ax
mov fs, ax

pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
xor rax, rax                 ; Call C function
xor rax, rax

  mov eax, [apic_base]              	; Apic Base in C-code
  mov dword [eax + 0x80	], 0x00		; Enable soft ints
  pop rax

  iretq                             ; Return to code

[GLOBAL apic_spurious]
apic_spurious:                      ; Spurious Interrupt for IOAPIC
iretq                               ; No EOI



%macro ISR_NOERRCODE 1              ; NAM Macros are awesome
global isr%1
  isr%1:
    cli                             ; Disable interrupts firstly
    push 0                          ; Push a dummy error code.
    push %1                         ; Push the interrupt number.
    jmp isr_common_stub             ; Go to our common handler code.
%endmacro

%macro ISR_ERRCODE 1
global isr%1
  isr%1:
    cli                             ; Disable interrupts.
    push %1                         ; Push the interrupt number
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
