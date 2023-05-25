#ifndef protocol_kernel_header
#define protocol_kernel_header

#ifdef __cplusplus
extern "C" {
#endif

#ifndef protocol_types
#include "types.h"
#endif

/* FS/OS informatin. */
#pragma pack(1)
typedef struct OS_info
{
    uint8       FS_type;
    uint8       OS_name[16];
    uint8       OS_version[6];
    uint8       OS_type;
	bool		in_production;
} _OS_info;
#pragma pop
const _OS_info *oinfo = (_OS_info *) 0x7D00;

#ifndef protocol_util
#include "util.h"
#endif

#ifdef using_vesa
//#include "vesa_api.h"
#elif defined(using_b8000)
#include "b8000_api.h"
#else
#include "b8000_api.h"
#endif

#ifndef FAMP_filesystem_api
#include "filesystem/filesystem_api.h"
#endif

/* Kernel starting/ending points. */
extern uint16 kernel_start;
extern uint16 kernel_end;

/* For user-convenience. */
#define starting_point __attribute__((section("__start")))

/* Memory address that are, without a doubt, able to be released to memory map ASAP. */
const uint8 *instant_free_addresses[] = {
    (const uint8 *)0x0500, 
    (const uint8 *)0x9000,
    (const uint8 *)0xA000 // Kernel would of been relocated by the program in `0x9000`
};

struct vbe_info_structure {
	char signature[4];	// must be "VESA" to indicate valid VBE support
	uint16 version;			// VBE version; high byte is major version, low byte is minor version
	uint32 oem;			// segment:offset pointer to OEM
	uint32 capabilities;		// bitfield that describes card capabilities
	uint32 video_modes;		// segment:offset pointer to list of supported video modes
	uint16 video_memory;		// amount of video memory in 64KB blocks
	uint16 software_rev;		// software revision
	uint32 vendor;			// segment:offset to card vendor string
	uint32 product_name;		// segment:offset to card model name
	uint32 product_rev;		// segment:offset pointer to product revision
	char reserved[222];		// reserved for future expansion
	char oem_data[256];		// OEM BIOSes store their strings in this area
};

/* `clear_screen` keeps track of the function name. */
void init_kernel(uint8 *function_name, uint8 color_theme)
{
	clear_screen("init_kernel", (color_theme >> 4) & 0x0F, color_theme & 0x0F);
	init_cursor_pos();
	uint8 *src;

	//clear_screen(function_name, (color >> 4) & 0x0F, color & 0x0F);
	print("FS Partition Header Start Tag: ", '\0');
	print(filesystem_pheader->header_start, '\n');
	print("Partition Type: ", 0);
	itoa(filesystem_pheader->partition_type, src, 10);
	print(src, ' ');
	print("(",0);
	print(names_for_each_type[filesystem_pheader->partition_type], ')');
	print("\n", 0);
	print("Starting LBA: 0x", 0);
	itoa(filesystem_pheader->starting_LBA, src, 16);
	print(src, '\n');
	print("Ending LBA: 0x", 0);
	itoa(filesystem_pheader->ending_LBA, src, 16);
	print(src, '\n');
	uint8 total_sectors = filesystem_pheader->ending_LBA - filesystem_pheader->starting_LBA;
	print("\tTotal Sectors Of: 0x", 0);
	itoa(total_sectors, src, 16);
	print(src, '\n');
	print("\tTotal Bytes: ", 0);
	uint16 total = total_sectors * 512;
	itoa(total, src, 10);
	print(src, '\n');
	print("Virtual Address: 0x", 0);
	itoa(filesystem_pheader->virtual_address, src, 16);
	print(src, '\n');
	print("Partition Address: 0x", 0);
	itoa(filesystem_pheader->partition_address, src, 16);
	print(src, '\n');
	print("Type Of FS: ", 0);
	itoa(filesystem_pheader->FS_type, src, 10);
	print(src, '(');
	print(names_for_each_FS[filesystem_pheader->FS_type], ')');
	print("\n", 0);
	print("FS Partition Header End Tag: ", 0);
	print(filesystem_pheader->header_end, '\n');
}

extern struct vbe_info_structure vbe_info_block;

#ifdef __cplusplus
}
#endif

#endif