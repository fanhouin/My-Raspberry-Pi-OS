#include <cpio.h>
#include <uart.h>
#include <string.h>


// void testprint(){
//     // char buf[MAX_SIZE];
//     char *place = (char *)CPIO_BASE;
//     for(int i = 0; i < 500; i++){
//         uart_send(place[i]);
//     }
//     // uart_puts(place); 
// }

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
    cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE;
    while(1){
        file_info info;
        parse_cpio_header(header, &info);
        if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
        uart_nbyte(info.filename, info.filename_size);
        uart_puts("\t");
        header = (cpio_newc_header *)(info.data + padding(info.datasize));
    }
    uart_puts("\n");
}


void cat(char *thefilename) {
    cpio_newc_header *header = (cpio_newc_header *)CPIO_BASE;
    while(1){
        file_info info;
        parse_cpio_header(header, &info);

        if(strncmp(info.filename, "TRAILER!!!", 10) == 0) break;
        else if(strncmp(info.filename, thefilename, strlen(thefilename)) == 0){
            uart_nbyte(info.data, info.datasize);
            uart_puts("\n");
            return;
        }

        header = (cpio_newc_header *)(info.data + padding(info.datasize));
    }
}






