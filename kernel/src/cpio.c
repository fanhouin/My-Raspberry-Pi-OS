#include <cpio.h>
#include <uart.h>
#include <string.h>
#include <malloc.h>
#include <irq.h>

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
    if(strncmp(header->c_magic, "070701", 6) != 0) return;
    info->filename = (char *)(header + 1);
    info->filename_size = hextoui(header->c_namesize, 8);
    info->datasize = hextoui(header->c_filesize, 8);
    info->data = (char *)header + padding(sizeof(cpio_newc_header)+info->filename_size);
}

void ls() {
    cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE_START;
    while(1){
        file_info info;
        parse_cpio_header(header, &info);
        if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
        uart_nbyte(info.filename, info.filename_size);
        uart_puts("\n");
        header = (cpio_newc_header *)(info.data + padding(info.datasize));
    }
}


void cat(char *thefilename) {
    cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE_START;
    while(1){
        file_info info;
        parse_cpio_header(header, &info);

        if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
        else if(strncmp(info.filename, thefilename, info.filename_size) == 0){
            uart_nbyte(info.data, info.datasize);
            uart_puts("\n");
            return;
        }

        header = (cpio_newc_header *)(info.data + padding(info.datasize));
    }
}


unsigned long findDataAddr(char *thefilename) {
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

void run(unsigned long runAddr){
    unsigned long *user_stack = (unsigned long*)simple_malloc(0x1000);
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




