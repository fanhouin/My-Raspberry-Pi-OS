#include <fat32.h>
#include <sddriver.h>
#include <uart.h>
#include <string.h>
#include <malloc.h>

struct fat32_info *fat32_info_pool;
struct fat32_info *curr_fat32_info;
struct file_operations *fat32_fops;
struct vnode_operations *fat32_vpos;
struct mbr_t *mbr;

// struct 

struct mbr_t *fat32_parse_mbr(){
    struct mbr_t *new_mbr = (struct mbr_t *)kmalloc(sizeof(struct mbr_t));
    readblock(0, mbr);

    uart_puts("========MBR========\n");
    for(int i = 0; i < 4; i++){
        uart_puts("partition ");
        print_string(UITOA, "", i, 0);
        print_string(UITOA, ": type code", mbr->partition[i].type_code, 0);
        print_string(UITOA, ", lba", mbr->partition[i].lba_begin, 0);
        print_string(UITOA, ", num_of_sectors", mbr->partition[i].num_of_sectors, 1);
    }
    return new_mbr;
}

struct fat32_boot_sector *fat32_parse_boot_sector(uint32_t lba_begin){
    struct fat32_boot_sector *tmp_boot_sector = (struct fat32_boot_sector*)kmalloc(sizeof(struct fat32_boot_sector));
    memset(tmp_boot_sector, NULL, sizeof(struct fat32_boot_sector));
    readblock(lba_begin, tmp_boot_sector);
    return tmp_boot_sector; 
}

struct fat32_dir_entry *fat32_parse_dir_entry(struct fat32_info info){
    
}

void fat32_init(){
    fat32_info_pool = (struct fat32_info *)kmalloc(sizeof(struct fat32_info) * 4);
    // parse mbr
    mbr = fat32_parse_mbr();
    for(int i = 0; i < 4; i++){
        /* if lba is 0, it means this partition is not used */
        if(mbr->partition[i].lba_begin != 0){
            fat32_info_pool[i].par_entry = &mbr->partition[i];
            fat32_info_pool[i].boot_sector = fat32_parse_boot_sector(mbr->partition[i].lba_begin);
            if(fat32_info_pool[i].boot_sector != NULL){
                fat32_info_pool[i].dir_entry = fat32_parse_dir_entry(&fat32_info_pool[i]);
            }
        }
    }
    
}