#ifndef FAMP_filesystem_api
#define FAMP_filesystem_api

/* Partition types.
 * The FS dwells in a partition; rather, the FS is a partition in and of itself.
 * FAMP would like to know how to handle the partition. To the protocol, a partition is a set of memory that has a description for it.
 * For the FS, there is what is called a `partition header` which gives more in-depth detail/information over the partition. With the `partition header`,
 * FAMP will be able to decipher whether the given partition is a FS or not(which should not be too hard).
 * There are aims to use a partition header in a more wide variety of ways if FAMP ever ends up supporting the extended partition table.
 * For now, if FAMP finds a partition header it will default to assuming it found the FS, since the last partition in the MBR partition table is where the FS (should) be located.
 */
enum partition_type {
    part_type_1     = 0, // Available to user/kernel
    part_type_2     = 1, // Available to kernel only
    part_type_3     = 2, // Critical data, available only to kernel
    part_type_4     = 3, // Critical data, available to user and kernel(useful for if user has some sort of data/file they want to keep secured)
    part_type_5     = 4, // Extra. The partition(in specific, the FS within the partition) doesn't care what it has/where it is used
};

/* Types of filesystems. */
enum FS_types {
    FS_fat32        = 0,
    FS_ext2         = 1,
    FS_FAMP_custom  = 2,
    FS_ISO9660      = 3
};

const uint8 *names_for_each_type[] = {
    "Available to user/kernel", "Available to kernel only", "Critical data, available only to kernel",
    "Critical data, available to user and kernel", "Extra. The partition(in specific, the FS within the partition) doesn't care what it has/where it is used"
};

const uint8 *names_for_each_FS[] = {
    "Fat32", "Ext2", "FAMP Custom", "ISO9660"
};

#pragma pack(1)
typedef struct ph
{
    /* Signify the start of partition header. */
    uint8            header_start[6]; // "PARTS  ", in binary: "P A R T S 00"

    /* Partition type(available to user, not available to user/available 
     * to user AND the kernel etc). 
     */
    uint8            partition_type;

    /* LBA values where partition starts/ends. */
    uint32           starting_LBA;
    uint32           ending_LBA;

    /* Address of partition(really, address of FS). */
    uint16           partition_address;

    /* Virtual address of partition. */
    uint32           virtual_address;

    /* What type of FS does the user want? FAMPs custom FS, or a pre-existing one? */
    uint8            FS_type;

    /* If `FS_type` is `FS_FAMP_custom` then what revision of FAMPs custom FS is currently being used? 
     * Set to zero if we are using a FS other than FAMPs custom FS. 
     */
    uint32           FAMP_custom_FS_revision;

    /* CHS(Cylinder, Head, Sector). Just in case we ever do need it.
     * A later revision of `struct PartitionHeader` will, more than likely, remove support for CHS.
     */
    uint16           cylinder;
    uint8            head;
    uint32           sector;

    /* 9 of the 15 bytes will be padded out with zero, 
     * the last 6 bytes will represent `PARTE  `, in binary: `P A R T E 00`(indicating the end of the partition header). 
     */
    uint8            header_end[15];
} _ph;
#pragma pop

extern _ph filesystem_pheader[];

#endif