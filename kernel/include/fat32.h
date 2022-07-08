#ifndef FAT32_H_
#define FAT32_H_
#include <stddef.h>
#include <vfs.h>

#define MBR_ENTRY_COUNT 4
#define BLOCK_SIZE 512

// some useful tutorial
// https://www.pjrc.com/tech/8051/ide/fat32.html

// Reference
// https://en.wikipedia.org/wiki/Master_boot_record -> Partition entry
struct partition_entry {
    uint8_t status;                         // Status or physical drive 
    uint8_t chs_first[3];                   // CHS address
    uint8_t type_code;                      // partition type: 0x0B or 0x0C
    uint8_t chs_last[3];                    // CHS address
    uint32_t lba_begin;                     // LBA of first absolute sector in the partition
    uint32_t num_of_sectors;                // number of sectors in the partition 
} __attribute__((packed));

struct mbr_t {
    char boot_code[446];
    struct partition_entry partition[4];
    char boot_signature[2];                 // always 0x55 and 0xAA     
} __attribute__((packed));


// Reference
// https://github.com/ShamariFeaster/FAT32-Implementation/blob/master/proj3-clean.c
// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#Bootsector
// http://www.independent-software.com/operating-system-development-boot-sector-structure.html
struct fat32_boot_sector{                         // Typical value
    char jump[3];                           // Jump instruction
    char os_name[8];                        // OEM Name

    // BIOS Parameter Block
    uint16_t sector_size;                   // 512 bytes
    uint8_t sectors_per_cluster;            // 1, 2, 4, 8, 16, 32, 64, 128
    uint16_t reserved_sectors;              // usually 0x20
    uint8_t number_of_fats;                 // always 2
    uint16_t root_entries;                  // 0 for FAT32
    uint16_t total_sectors_16;              // 0 for FAT32
    uint8_t media_descriptor;               // 0xF8 for FAT32
    uint16_t sectors_per_fat;               // usually 3 for FAT32

    // DOS3.31 BPB
    uint16_t sectors_per_track;             // usually 63 for FAT32
    uint16_t num_of_heads;                  // usually 255 for FAT32
    uint32_t num_of_hidden_sectors;         // usually 0 for FAT32
    uint32_t num_of_sectors_32;             // usually 0 for FAT32
    
    // FAT32 Extended BIOS Parameter Block
    uint32_t num_of_hidden_sectors_32;      // 0 for FAT32
    uint16_t flag;                          // 0xF00 for FAT32
    uint32_t version;                       // 0x0000 for FAT32
    uint32_t root_cluster;                  // 2 for FAT32
    uint16_t fsinfo_sector;                 // 1 for FAT32
    uint16_t backup_boot_sector;            // 6 for FAT32
    char reserved[12];                      // 0 for FAT32
    uint8_t drive_number;                   // 0 for FAT32
    uint8_t reserved_2;                     // 0 for FAT32
    uint8_t boot_signature;                 // 0x29 for FAT32
    uint32_t volume_id;                     // 0 for FAT32
    char volume_label[11];                  // "NO NAME" for FAT32
    char fs_type[8];                        // FAT32
}__attribute__((packed));


// Reference
// https://en.wikipedia.org/wiki/8.3_filename
struct fat32_dir_entry{
    char name[8]; 
    char extension[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t create_time_ms;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t last_modify_time;
    uint16_t last_modify_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
}__attribute__((packed));

struct fat32_info{
    struct partition_entry *par_entry;
    struct fat32_boot_sector *boot_sector;
    struct fat32_dir_entry *dir_entry;
};

void fat32_init();

struct mbr_t *fat32_parse_mbr();
struct fat32_boot_sector *fat32_parse_boot_sector(uint32_t lba_begin);
struct fat32_dir_entry *fat32_parse_dir_entry(struct fat32_info info);


#endif