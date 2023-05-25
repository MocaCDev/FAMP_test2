
%macro __x86_EnterRealMode 0
    [bits 32]
    jmp word 18h:.__pmode16         ; 1 - jump to 16-bit protected mode segment

.__pmode16:
    [bits 16]
    ; 2 - disable protected mode bit in cr0
    mov eax, cr0
    and al, ~1
    mov cr0, eax

    ; 3 - jump to real mode
    jmp word 00h:.__rmode

.__rmode:
    ; 4 - setup segments
    mov ax, 0x0
    mov ds, ax
    mov ss, ax

    ; 5 - enable interrupts
    sti

%endmacro

global enter_protected_mode
%macro enter_protected_mode 0
    cli

    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp dword 0x08:.pmode
.pmode:
    use32
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

%endmacro

;global test_entering
;test_entering:
;use32
;    __x86_EnterRealMode
;
;    use16
;    pusha
;
;    mov ah, 0x0E
;    mov al, 'A'
;    int 0x10
;
;    popa
;    enter_protected_mode
;    use32
;    ret