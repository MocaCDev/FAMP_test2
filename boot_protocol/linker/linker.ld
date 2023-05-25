ENTRY(main)
SECTIONS {
    /* Critical memory addresses for FAMP. */
    g_gdt_address           = 0x0900;
    g_gdt_desc_address      = 0x092A;
    g_gdt_status_address    = 0x0938;

    /* Memory stamp addresses for second stage bootloader and kernel. */
    ss_mem_stamp_addr       = 0x9000;
    kernel_mem_stamp_addr   = 0x10200;

    /* Kernel address/kernel size for second stage bootloader. */
    kernel_size             = 25600;
    kernel_address          = 0xA000;

    /* Second stage starts at 0x7E00. */
    . = 0x7E00;
    second_stage_start = .;

    .text : AT(0x7E00)
    {
        *(__start*);
        *(.text*);
    }

    .data :
    {
        *(.bss*);
        *(.data*);
        *(.rodata*);
        *(COMMON*);
    }


    second_stage_end = .;
}