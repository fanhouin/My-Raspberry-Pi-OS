#ifndef CPIO_H_
#define CPIO_H_

#include <string.h>
// #define CPIO_BASE   ((volatile unsigned long*)(0x8000000))
unsigned long CPIO_BASE;

/* Magic identifiers for the "cpio" file format. */
#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define CPIO_ALIGNMENT 4


typedef struct _cpio_newc_header{
   	char c_magic[6];      /* Magic header '070701'. */
    char c_ino[8];        /* "i-node" number. */
    char c_mode[8];       /* Permisions. */
    char c_uid[8];        /* User ID. */
    char c_gid[8];        /* Group ID. */
    char c_nlink[8];      /* Number of hard links. */
    char c_mtime[8];      /* Modification time. */
    char c_filesize[8];   /* File size. */
    char c_dev1major[8];  /* Major dev number. */
    char c_devminor[8];   /* Minor dev number. */
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];   /* Length of filename in bytes. */
    char c_check[8];      /* Checksum. */
}cpio_newc_header;



typedef struct _file_info{
    char *filename;
    char *data;
    unsigned int filename_size;
    unsigned int datasize;
}file_info;

void testprint();
unsigned int padding(unsigned int);
void ls();
void cat(char [MAX_SIZE]);
void parse_cpio_header(cpio_newc_header *, file_info *);



#endif
