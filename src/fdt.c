#include <uart.h>
#include <fdt.h>
#include <cpio.h>

extern unsigned long CPIO_BASE;

uint32_t big_to_little(uint32_t big){
    uint32_t little = 0;
    little |= (big & 0xFF000000) >> 24;
    little |= (big & 0x00FF0000) >> 8;
    little |= (big & 0x0000FF00) << 8;
    little |= (big & 0x000000FF) << 24;
    return little;
}



// {
// 	chosen {
// 		linux,initrd-start = <0x82000000>;
// 		linux,initrd-end = <0x82800000>;
// 	};
// };

void initramfs_callback(char * prop_name, uint32_t token_type, uint32_t len, uint32_t *struct_addr){
    if(token_type == FDT_PROP){
        if(strcmp("linux,initrd-start", prop_name) == 0){
            char buf[15];
            CPIO_BASE = (unsigned long)big_to_little(*(struct_addr+2));
            uart_puts("[*] CPIO_BASE: 0x");
            uitohex(buf, (unsigned int)(CPIO_BASE));
            uart_puts(buf);
            uart_puts("\n");
        }
    }
}

void fdt_traverse(fdt_header *header, dtb_callback the_callback){
    if(big_to_little(header->magic) != FDT_MAGIC){
        uart_puts("[x] fdt_traverse: wrong magic number\n");
        return;
    }
    uint32_t *struct_addr = (uint32_t *)((char *)header + big_to_little(header->off_dt_struct));
    char *strings_addr = (char *)header + big_to_little(header->off_dt_strings);
    char *node_name;

    while(1){
        uint32_t token_type = big_to_little(*struct_addr);
        if(token_type == FDT_END){
            break;
        }
        
        struct_addr += 1; // skip token type
        if(token_type == FDT_BEGIN_NODE){
            node_name = (char *)(struct_addr); // chosen
            int node_name_size = strlen(node_name) + 1; // The node’s name as a '\0' string
            struct_addr += padding(node_name_size) / 4;
        }
        else if(token_type == FDT_PROP){
            uint32_t len = big_to_little(*(uint32_t *)struct_addr);
            uint32_t nameoff = big_to_little(*(uint32_t *)(struct_addr + 1));
            char *prop_name = strings_addr + nameoff;
            the_callback(prop_name, token_type, len, struct_addr);

            struct_addr += 2;
            struct_addr += padding(len) / 4;
        }
        else if(token_type == FDT_NOP || token_type == FDT_END_NODE){
            // do something
        }
        else{
            uart_puts("fdt_traverse: wrong token type\n");
            return;
        }
    }

}



