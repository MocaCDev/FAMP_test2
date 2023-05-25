#define using_b8000
#include "boot_protocol/protocol/kernel_header.h"

typedef struct test
{
    uint8 age;
} _test;

extern _ph filesystem[];
extern bool testIT(_test);
//extern _ph filesystem[];
void starting_point main()
{
    init_kernel("main", create_color_comb(red, black));

    uint8 key = 0;
    while(1)
    {
        key = get_char(NULL, false);
        if(key == 0x01) break;
    }

    /* The first byte in the program `mbr_partition_table.c`(binary `mbr_partition_tabe.bin`)
     * should have the value 0x66.
     */
    if(instant_free_addresses[0][0] == 0x66)
        print("\n\n\n\tHello, @r World@re\n\t\t*evil laugh*", 0);

    /* Halt. */
    while(1);
}