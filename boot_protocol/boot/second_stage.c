#define bit32_bit16_GDT
#define default_gdt

#include "../protocol/boot_header.h"

void starting_point main()
{
    init_bootloader();

    /* Start from where the expected end is at. This is where the memory stamp is. Save us some runtime. */
    _memory_stamp *second_stage_mem_stamp = __obtain_memory_stamp(ss_mem_stamp_addr, second_stage);
    _memory_stamp *kernel_mem_stamp = __obtain_memory_stamp(kernel_mem_stamp_addr, kernel);

    load_kernel();
}