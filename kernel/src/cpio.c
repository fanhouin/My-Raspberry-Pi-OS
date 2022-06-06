#include <cpio.h>
#include <uart.h>
#include <string.h>
#include <malloc.h>
#include <irq.h>
#include <malloc.h>

file_info **cpio_file_info_list;
// TODO: make functions change faster, beacuse has the cpio_file_info_list
/*
The pathname is followed by NUL bytes so that the total size of the fixed
header plus pathname is a multiple	of four.  Likewise, the	file data is
padded to a multiple of four bytes.  Note that this format	supports only
4 gigabyte	files (unlike the older	ASCII format, which supports 8 giga-
byte files).
*/
unsigned int padding(unsigned int size){
    unsigned int padding = size % 4;
    if(padding == 0) return size;
    else return size + 4 - padding;
}

void parse_cpio_header(cpio_newc_header *header, file_info *info){
    if(strncmp(header->c_magic, CPIO_HEADER_MAGIC, 6) != 0) return;
    info->filename = (char *)(header + 1);
    info->filename_size = hextoui(header->c_namesize, 8);
    info->datasize = hextoui(header->c_filesize, 8);
    info->data = (char *)header + padding(sizeof(cpio_newc_header)+info->filename_size);
    info->c_nlink = header->c_nlink;
}

void init_cpio_file_info(){
    cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE_START;
    cpio_file_info_list = (file_info **)kmalloc(sizeof(file_info *) * MAX_CPIO_FILE_NUM);
    int i = 0;
    while(1){
        file_info *info = (file_info *)kmalloc(sizeof(file_info));
        parse_cpio_header(header, info);
        if(strncmp(info->filename, CPIO_FOOTER_MAGIC, 10) == 0) break;
        header = (cpio_newc_header *)(info->data + padding(info->datasize));
        cpio_file_info_list[i++] = info;
    }
}

void ls() {
    for(int i = 0; cpio_file_info_list[i] != NULL; i++){
        uart_puts(cpio_file_info_list[i]->filename);
        print_string(UITOA, " | datasize = ", cpio_file_info_list[i]->datasize, 0);
        uart_puts(" | ");
        uart_nbyte(cpio_file_info_list[i]->c_nlink, 8);
        uart_puts("\n");
    }
    // cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE_START;
    // while(1){
    //     file_info info;
    //     parse_cpio_header(header, &info);
    //     if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
    //     uart_nbyte(info.filename, info.filename_size);
    //     print_string(UITOA, " data_size: ", info.datasize, 1);
    //     uart_puts("\n");
    //     header = (cpio_newc_header *)(info.data + padding(info.datasize));
    // }
}


void cat(char *thefilename) {
    for(int i = 0; cpio_file_info_list[i] != NULL; i++){
        file_info *info = cpio_file_info_list[i];
        if(strncmp(info->filename, thefilename, info->filename_size) == 0){
            uart_nbyte(info->data, info->datasize);
            uart_puts("\n");
            return;
        }
    }
    // cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE_START;
    // while(1){
    //     file_info info;
    //     parse_cpio_header(header, &info);

    //     if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
    //     else if(strncmp(info.filename, thefilename, info.filename_size) == 0){
    //         uart_nbyte(info.data, info.datasize);
    //         uart_puts("\n");
    //         return;
    //     }

    //     header = (cpio_newc_header *)(info.data + padding(info.datasize));
    // }
}


unsigned long findDataAddr(const char *thefilename) {
    cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE_START;
    while(1){
        file_info info;
        parse_cpio_header(header, &info);

        if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
        else if(strncmp(info.filename, thefilename, info.filename_size) == 0){
            return (unsigned long)info.data;
        }

        header = (cpio_newc_header *)(info.data + padding(info.datasize));
    }
    return 0;
}

file_info cpio_find_file_info(const char *thefilename) {
    cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE_START;
    file_info info;
    while(1){
        parse_cpio_header(header, &info);

        if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
        else if(strncmp(info.filename, thefilename, info.filename_size) == 0){
            return info;
        }

        header = (cpio_newc_header *)(info.data + padding(info.datasize));
    }
    info.filename = 0;
    return info;

}

void run(unsigned long runAddr){
    unsigned long *user_stack = (unsigned long *)kmalloc(0x1000);
    set_period_timer_irq();
    asm volatile(
        "mov x0, 0x0\n\t"
        "msr spsr_el1, x0\n\t"
        "msr elr_el1, %0\n\t"
        "msr sp_el0, %1\n\t"
        "eret\n\t"
        ::"r"(runAddr), 
        "r"(user_stack)
        : "x0"
    );
    uart_puts("run done\n"); 
}




