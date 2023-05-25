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

test_struc:
    .age db 0

use16
test_the_struct:
    mov ah, [test_struc.age]
    cmp ah, 0x0
    jne .h
    mov [test_struc], di

    mov ah, [test_struc.age]
.h:
    cmp ah, 0x05
    je .y
    ret
.y:
    mov ah, 0x0E
    mov al, 'T'
    int 0x10
    ret


global testIT
testIT:
    use32
    push ebp
    mov ebp, esp

    __x86_EnterRealMode
    use16

    mov di, [ebp + 8]
    
    mov eax, true

    push eax
    enter_protected_mode
    use32
    pop eax
    mov esp, ebp
    pop ebp

    ret

global vbe_info_block
vbe_info_block:
	.vbe_signature:			db 'VBE2'
	.vbe_version:			dw 0x0
	.oem_string_pointer:		dd 0x0
	.capabilities:			dd 0x0
	.video_modes:			dd 0x0
	.total_memory:			dw 0x0
	.oem_software_rev:		dw 0x0
	.oem_vendor_name_pointer:	dd 0x0
	.oem_product_name_pointer:	dd 0x0
	.oem_product_revision_pointer:	dd 0x0
	.reserved:	times 222 db 0
	.oem_data:	times 256 db 0
	

mode_info_block:
	.attributes:		dw 0x0	; 0-2 byte
	.window_a:		db 0x0	; 2-3 byte
	.window_b:		db 0x0	; 3-4 byte
	.granularity:		dw 0x0	; 4-6 byte
	.window_size:		dw 0x0	; 6-8 byte
	.segment_a:		dw 0x0	; 8-10 byte
	.segment_b:		dw 0x0	; 10-12 byte
	.win_func_ptr:		dd 0x0	; 12-16 byte
	.pitch:			dw 0x0	; 16-18 byte
	.width:			dw 0x0	; 18-20 byte
	.height:		dw 0x0	; 20-22 byte
	.w_char:		db 0x0	; 22-23 byte
	.y_char:		db 0x0	; 23-24 byte
	.planes:		db 0x0	; 24-25 byte
	.bpp:			db 0x0	; 25-26 byte
	.banks:			db 0x0	; 26-27 byte
	.memory_model:		db 0x0	; 27-28 byte
	.bank_size:		db 0x0	; 28-29 byte
	.image_pages:		db 0x0	; 29-30 byte
	.reserved1:		db 0x1	; 30-31 byte
	.red_mask_size:		db 0x0	; 31-32 byte
	.red_field_pos:		db 0x0	; 32-33 byte
	.green_mask_size:	db 0x0	; 33-34 byte
	.green_field_pos:	db 0x0	; 34-35 byte
	.blue_mask_size:	db 0x0	; 35-36 byte
	.blue_field_pos:	db 0x0	; 36-37 byte
	.reserved_mask_size:	db 0x0	; 37-38 byte
	.reserved_field_pos:	db 0x0	; 38-39 byte
	.direct_color_mode_info:db 0x0	; 39-40 byte
	.physical_base_ptr:	dd 0x0	; 40-44 byte
	.reserved2:		dd 0x0	; 44-48 byte
	.reserved3:		dw 0x0	; 48-50 byte
	.linear_bytes_psl:	dw 0x0	; 50-52 byte
	.bank_number_of_ip:	db 0x0	; 52-53 byte
	.linear_number_of_ip:	db 0x0	; 53-54 byte
	.linear_red_mask_size:	db 0x0	; 54-55 byte
	.linear_red_field_pos:	db 0x0	; 55-56 byte
	.linear_green_mask_size:db 0x0	; 56-57 byte
	.linear_green_field_pos:db 0x0	; 57-58 byte
	.linear_blue_mask_size: db 0x0	; 58-59 byte
	.linear_blue_field_pos: db 0x0	; 59-60 byte
	.linear_res_mask_sie:   db 0x0	; 60-61 byte
	.linear_res_field_pos:	db 0x0	; 61-62 byte
	.max_pixel_clock:	dd 0x0	; 62-66 byte
	.reserved4: times 190 db 0

;times 2048 - ($ - $$) db 0 