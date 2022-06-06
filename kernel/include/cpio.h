#ifndef CPIO_H_
#define CPIO_H_

#include <string.h>
// #define CPIO_BASE   ((volatile unsigned long*)(0x8000000))
unsigned long long CPIO_BASE_START;
unsigned long long CPIO_BASE_END;

/* Magic identifiers for the "cpio" file format. */
#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define CPIO_ALIGNMENT 4
#define MAX_CPIO_FILE_NUM 1024


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
    char *c_nlink;
}file_info;

void init_cpio_file_info();
unsigned int padding(unsigned int);
void ls();
void cat(char [MAX_SIZE]);
void run(unsigned long);
unsigned long findDataAddr(const char *);
file_info cpio_find_file_info(const char *thefilename);
void parse_cpio_header(cpio_newc_header *, file_info *);


#endif
