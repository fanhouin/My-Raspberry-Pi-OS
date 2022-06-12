/*
  FUSE ssd: FUSE ioctl example
  Copyright (C) 2008       SUSE Linux Products GmbH
  Copyright (C) 2008       Tejun Heo <teheo@suse.de>
  This program can be distributed under the terms of the GNU GPLv2.
  See the file COPYING.
*/
#define FUSE_USE_VERSION 35
#include <fuse.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "ssd_fuse_header.h"
#define SSD_NAME       "ssd_file"
enum
{
    SSD_NONE,
    SSD_ROOT,
    SSD_FILE,
};


static size_t physic_size;
static size_t logic_size;
static size_t host_write_size;
static size_t nand_write_size;

typedef union pca_rule PCA_RULE;
union pca_rule
{
    unsigned int pca;
    struct
    {
        unsigned int lba : 16;
        unsigned int nand: 16;
    } fields;
};

PCA_RULE curr_pca;
// PCA_RULE my_pca;
static unsigned int get_next_pca();

unsigned int* L2P,* P2L,* valid_count, free_block_number;
LRU *lru_table;
LRU *lru_head;

static int ssd_resize(size_t new_size)
{
    //set logic size to new_size
    if (new_size > NAND_SIZE_KB * 1024)
    {
        return -ENOMEM;
    }
    else
    {
        logic_size = new_size;
        return 0;
    }

}

static int ssd_expand(size_t new_size)
{
    //logic must less logic limit

    if (new_size > logic_size)
    {
        return ssd_resize(new_size);
    }

    return 0;
}

static int nand_read(char* buf, int pca)
{
    char nand_name[100];
    FILE* fptr;

    PCA_RULE my_pca;
    my_pca.pca = pca;
    snprintf(nand_name, 100, "%s/nand_%d", NAND_LOCATION, my_pca.fields.nand);

    //read
    if ( (fptr = fopen(nand_name, "r") ))
    {
        fseek( fptr, my_pca.fields.lba * 512, SEEK_SET );
        fread(buf, 1, 512, fptr);
        fclose(fptr);
    }
    else
    {
        printf("open file fail at nand read pca = %d\n", pca);
        return -EINVAL;
    }
    return 512;
}
static int nand_write(const char* buf, int pca)
{
    char nand_name[100];
    FILE* fptr;

    PCA_RULE my_pca;
    my_pca.pca = pca;
    snprintf(nand_name, 100, "%s/nand_%d", NAND_LOCATION, my_pca.fields.nand);

    //write
    if ( (fptr = fopen(nand_name, "r+")))
    {
        fseek( fptr, my_pca.fields.lba * 512, SEEK_SET );
        fwrite(buf, 1, 512, fptr);
        fclose(fptr);
        physic_size ++;
        valid_count[my_pca.fields.nand]++;
        printf("my_pca.fields.nand = %d | valid_count = %d\n", my_pca.fields.nand,valid_count[my_pca.fields.nand]);
    }
    else
    {
        printf("open file fail at nand (%s) write pca = %d, return %d\n", nand_name, pca, -EINVAL);
        return -EINVAL;
    }

    nand_write_size += 512;
    return 512;
}

static int nand_erase(int block_index)
{
    char nand_name[100];
    FILE* fptr;
    snprintf(nand_name, 100, "%s/nand_%d", NAND_LOCATION, block_index);
    fptr = fopen(nand_name, "w");
    if (fptr == NULL)
    {
        printf("erase nand_%d fail", block_index);
        return 0;
    }
    fclose(fptr);
    valid_count[block_index] = FREE_BLOCK;
    return 1;
}



static unsigned int get_next_block()
{
    for (int i = 0; i < PHYSICAL_NAND_NUM; i++)
    {
        if (valid_count[(curr_pca.fields.nand + i) % PHYSICAL_NAND_NUM] == FREE_BLOCK)
        {
            curr_pca.fields.nand = (curr_pca.fields.nand + i) % PHYSICAL_NAND_NUM;
            curr_pca.fields.lba = 0;
            // printf("curr_pca.fields.nand = %d | curr_pca.fields.lba = %d\n",curr_pca.fields.nand, curr_pca.fields.lba);
            // printf("curr_pca.pca = %d\n", curr_pca.pca);
            free_block_number--;
            valid_count[curr_pca.fields.nand] = 0;
            return curr_pca.pca;
        }
    }
    return OUT_OF_BLOCK;
}
static unsigned int get_next_pca()
{
    if (curr_pca.pca == INVALID_PCA)
    {
        //init
        curr_pca.pca = 0;
        valid_count[0] = 0;
        free_block_number--;
        return curr_pca.pca;
    }

    if(curr_pca.fields.lba == 9)
    {
        int temp = get_next_block();
        if (temp == OUT_OF_BLOCK)
        {
            return OUT_OF_BLOCK;
        }
        else if(temp == -EINVAL)
        {
            return -EINVAL;
        }
        else
        {
            return temp;
        }
    }
    else
    {
        curr_pca.fields.lba += 1;
    }
    return curr_pca.pca;

}

/* update lru */
static void update_lru(int block){
    list_del(&lru_table[block].list);
    INIT_LIST_HEAD(&lru_table[block].list);
    list_add(&lru_table[block].list, &lru_head->list);
    
    struct list_head *pos;
    list_for_each(pos, &lru_head->list)
    {
        LRU *tmp = (LRU*)pos;
        printf("%d -> ", tmp->idx);
    }
    printf("update block = %d\n", block);
}

static int ftl_read( char* buf, size_t lba)
{
    // TODO
    // printf("read pca = %d\n", L2P[lba]);
    int pca = L2P[lba];
    if(pca == INVALID_PCA){
        printf("[x] FTL: don't have pca for lba = %ld\n", lba);
        return INVALID_PCA;
    }
    int size = nand_read(buf, pca);
    if(size == -EINVAL){
        return -EINVAL;
    }

    /* update lru */
    // update_lru(pca >> 16);

    return size;
}

static int ftl_write(const char* buf, size_t lba_range, size_t lba, int read_mod_write)
{
    // TODO
    int override = 1;
    int pca = L2P[lba];
    printf("[*] before pca = %d",pca);
    if(pca == INVALID_PCA){
        L2P[lba] = get_next_pca();
        pca = L2P[lba];
        override = 0;
    }
    if(pca == OUT_OF_BLOCK){
        printf("[x] write fail at lba = %ld\n", lba);
        return OUT_OF_BLOCK;
    }
    printf(" | after pca = %d\n", pca);

    int size = nand_write(buf, pca);
    if(size == -EINVAL){
        return -EINVAL;
    }
    
    /* 
     * udpate P2L table
     * physical_nand = 13, logical_nand = 10, need to use pca>>16 to select which block
    */
    P2L[((pca >> 16) * PAGE_PER_BLOCK) + (lba % PAGE_PER_BLOCK)] = lba;

    /* if override the page, need to sub the valid_count */
    if(override || read_mod_write) {
        valid_count[pca >> 16]--;
    }

    
    /* update lru */
    // update_lru(pca >> 16);

    // printf("P2L idx = %d\n", ((pca >> 16) * PAGE_PER_BLOCK) + (lba % PAGE_PER_BLOCK));
    return size;
}



static int ssd_file_type(const char* path)
{
    if (strcmp(path, "/") == 0)
    {
        return SSD_ROOT;
    }
    if (strcmp(path, "/" SSD_NAME) == 0)
    {
        return SSD_FILE;
    }
    return SSD_NONE;
}
static int ssd_getattr(const char* path, struct stat* stbuf,
                       struct fuse_file_info* fi)
{
    (void) fi;
    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_atime = stbuf->st_mtime = time(NULL);
    switch (ssd_file_type(path))
    {
        case SSD_ROOT:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;
        case SSD_FILE:
            stbuf->st_mode = S_IFREG | 0644;
            stbuf->st_nlink = 1;
            stbuf->st_size = logic_size;
            break;
        case SSD_NONE:
            return -ENOENT;
    }
    return 0;
}
static int ssd_open(const char* path, struct fuse_file_info* fi)
{
    (void) fi;
    if (ssd_file_type(path) != SSD_NONE)
    {
        return 0;
    }
    return -ENOENT;
}
static int ssd_do_read(char* buf, size_t size, off_t offset)
{
    int tmp_lba, tmp_lba_range, rst ;
    char* tmp_buf;

    //off limit
    if ((offset ) >= logic_size)
    {
        return 0;
    }
    if ( size > logic_size - offset)
    {
        //is valid data section
        size = logic_size - offset;
    }

    tmp_lba = offset / 512;
    tmp_lba_range = (offset + size - 1) / 512 - (tmp_lba) + 1;
    tmp_buf = calloc(tmp_lba_range * 512, sizeof(char));

    for (int i = 0; i < tmp_lba_range; i++) {
        // TODO
        rst = ftl_read(tmp_buf + i * 512, tmp_lba + i);
        if (rst == -EINVAL){
            return -EINVAL;
        } 
        else if(rst == INVALID_PCA){
            return INVALID_PCA;
        }
    }

    memcpy(buf, tmp_buf + offset % 512, size);

    
    free(tmp_buf);
    return size;
}
static int ssd_read(const char* path, char* buf, size_t size,
                    off_t offset, struct fuse_file_info* fi)
{
    (void) fi;
    if (ssd_file_type(path) != SSD_FILE)
    {
        return -EINVAL;
    }
    return ssd_do_read(buf, size, offset);
}
static int ssd_do_write(const char* buf, size_t size, off_t offset)
{
    int idx = 0;
    int tmp_lba, tmp_lba_range, process_size;
    int curr_size, remain_size, re;
    char* tmp_buf;
    gc();


    off_t lba_offset = offset % 512;

    host_write_size += size;
    if (ssd_expand(offset + size) != 0)
    {
        return -ENOMEM;
    }

    tmp_lba = offset / 512;
    tmp_lba_range = (offset + size - 1) / 512 - (tmp_lba) + 1;
    process_size = 0;
    remain_size = size;
    curr_size = 0;

    /* read-modify-write */
    if(lba_offset > 0){
        off_t read_offset = offset - lba_offset;
        curr_size = remain_size > 512 ? 512 : remain_size;
        curr_size -= lba_offset;
        tmp_buf = calloc(512, sizeof(char));
        printf("[*] read_off = %ld\n", read_offset);
        printf("[*] lba_offset = %ld | remain_size = %d | curr_size = %d\n", lba_offset, remain_size, curr_size);

        re = ssd_do_read(tmp_buf, 512, read_offset);
        if(re == -EINVAL){
            return -EINVAL;
        }

        memcpy(tmp_buf + lba_offset, buf, curr_size);

        /*
         * if re == INVALID_PCA, valid_count++ 
         * if re != INVALID_PCA, don't change valid_count because the page has data already;
         */
        if(re == INVALID_PCA){
            re = ftl_write(tmp_buf, tmp_lba_range, tmp_lba, 0);
        }
        else{
            re = ftl_write(tmp_buf, tmp_lba_range, tmp_lba, 1);
        }

        if(re == -EINVAL){
            return -EINVAL;
        }

        // valid_count[curr_pca.fields.nand]--;
        process_size += curr_size;
        remain_size -= curr_size;
        idx = 1;
        free(tmp_buf);
    }

    for (; idx < tmp_lba_range; idx++)
    {    // TODO
        // printf("[*] lba_offset = %ld | remain_size = %d | curr_size = %d\n", lba_offset, remain_size, curr_size);
        curr_size = remain_size > 512 ? 512 : remain_size;
        tmp_buf = calloc(512, sizeof(char));
        memcpy(tmp_buf, buf + process_size, curr_size);
        re = ftl_write(tmp_buf, tmp_lba_range, tmp_lba + idx, 0);
        if(re == -EINVAL){
            return -EINVAL;
        }
        // valid_count[my_pca.fields.nand]++;
        process_size += curr_size;
        remain_size -= curr_size;
        free(tmp_buf);
    }

    return size;
}
static int ssd_write(const char* path, const char* buf, size_t size,
                     off_t offset, struct fuse_file_info* fi)
{

    (void) fi;
    if (ssd_file_type(path) != SSD_FILE)
    {
        return -EINVAL;
    }
    return ssd_do_write(buf, size, offset);
}


int gc(){
    printf("=============================[gc start]=============================\n");
    int min = 999;
    int first = 1, min_block = 0, new_block_idx = 0;
    int pca, del_idx = 0;
    size_t lba;
    char *tmp_buf;
    int *del_buf;
    
    // while(1){
        del_buf = (int *)calloc(PAGE_PER_BLOCK, sizeof(int));
        /* can be optimized? */
        for(int i = 0; i < PHYSICAL_NAND_NUM; i++){
            printf("%d -> ",valid_count[i]);
            // if(valid_count[i] == FREE_BLOCK) invalid_idx = i;
            if(valid_count[i] < min && valid_count[i] > 0){
                min = valid_count[i];
                min_block = i;
            }
        }
        printf("\n");
        if(min >= 10 || (min > new_block_idx && new_block_idx > 0)) goto GC_END;


        for(; new_block_idx < PAGE_PER_BLOCK; new_block_idx++){
            if(P2L[min_block * PAGE_PER_BLOCK + new_block_idx] == INVALID_PCA) continue;
            lba = P2L[min_block * PAGE_PER_BLOCK + new_block_idx];
            tmp_buf = (char *)calloc(512, sizeof(char));
            ssd_do_read(tmp_buf, 512, lba * 512);

            if(first){
                pca = get_next_block();
                first = 0;
            
                if(pca == OUT_OF_BLOCK){
                    printf("[x] gc: pca OUT_OF_BLOCK\n");
                    return OUT_OF_BLOCK;
                }
            }
            else{
                pca = get_next_pca();
            }
            /* do_write in new block of the idx */
            int size = nand_write(tmp_buf, pca);
            if(size == -EINVAL){
                return -EINVAL;
                free(tmp_buf);
            }

            /* if write success, udpate L2P & P2L table & del_buf*/
            L2P[lba] = pca; 
            P2L[((pca >> 16) * PAGE_PER_BLOCK) + (lba % PAGE_PER_BLOCK)] = lba;
            del_buf[del_idx++] = min_block * PAGE_PER_BLOCK + new_block_idx; 
            printf("move %d to %ld\n", min_block * PAGE_PER_BLOCK + new_block_idx, ((pca >> 16) * PAGE_PER_BLOCK) + (lba % PAGE_PER_BLOCK));
            free(tmp_buf);
        }
        for(int i = 0; i < del_idx; i++){
            P2L[del_buf[i]] = INVALID_LBA;
        }
        nand_erase(min_block);
        free(del_buf);
    // }

GC_END:
    printf("=============================[gc end]=============================\n");
    return 0;

}


static int ssd_truncate(const char* path, off_t size,
                        struct fuse_file_info* fi)
{
    (void) fi;
    memset(L2P, INVALID_PCA, sizeof(int) * LOGICAL_NAND_NUM * PAGE_PER_BLOCK);
    memset(P2L, INVALID_LBA, sizeof(int) * PHYSICAL_NAND_NUM * PAGE_PER_BLOCK);
    memset(valid_count, FREE_BLOCK, sizeof(int) * PHYSICAL_NAND_NUM);
    curr_pca.pca = INVALID_PCA;
    free_block_number = PHYSICAL_NAND_NUM;
    if (ssd_file_type(path) != SSD_FILE)
    {
        return -EINVAL;
    }

    return ssd_resize(size);
}
static int ssd_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info* fi,
                       enum fuse_readdir_flags flags)
{
    (void) fi;
    (void) offset;
    (void) flags;
    if (ssd_file_type(path) != SSD_ROOT)
    {
        return -ENOENT;
    }
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, SSD_NAME, NULL, 0, 0);
    return 0;
}
static int ssd_ioctl(const char* path, unsigned int cmd, void* arg,
                     struct fuse_file_info* fi, unsigned int flags, void* data)
{

    if (ssd_file_type(path) != SSD_FILE)
    {
        return -EINVAL;
    }
    if (flags & FUSE_IOCTL_COMPAT)
    {
        return -ENOSYS;
    }
    switch (cmd)
    {
        case SSD_GET_LOGIC_SIZE:
            *(size_t*)data = logic_size;
            return 0;
        case SSD_GET_PHYSIC_SIZE:
            *(size_t*)data = physic_size;
            return 0;
        case SSD_GET_WA:
            *(double*)data = (double)nand_write_size / (double)host_write_size;
            return 0;
    }
    return -EINVAL;
}
static const struct fuse_operations ssd_oper =
{
    .getattr        = ssd_getattr,
    .readdir        = ssd_readdir,
    .truncate       = ssd_truncate,
    .open           = ssd_open,
    .read           = ssd_read,
    .write          = ssd_write,
    .ioctl          = ssd_ioctl,
};
int main(int argc, char* argv[])
{
    int idx;
    char nand_name[100];
    physic_size = 0;
    logic_size = 0;
    curr_pca.pca = INVALID_PCA;
    free_block_number = PHYSICAL_NAND_NUM;

    L2P = malloc(LOGICAL_NAND_NUM * PAGE_PER_BLOCK * sizeof(int));
    memset(L2P, INVALID_PCA, sizeof(int) * LOGICAL_NAND_NUM * PAGE_PER_BLOCK);
    P2L = malloc(PHYSICAL_NAND_NUM * PAGE_PER_BLOCK * sizeof(int));
    memset(P2L, INVALID_LBA, sizeof(int) * PHYSICAL_NAND_NUM * PAGE_PER_BLOCK);
    valid_count = malloc(PHYSICAL_NAND_NUM * sizeof(int));
    memset(valid_count, FREE_BLOCK, sizeof(int) * PHYSICAL_NAND_NUM);

    /* init lru table */
    lru_table = (LRU *)malloc(PHYSICAL_NAND_NUM * sizeof(LRU));
    lru_head = (LRU *)malloc(sizeof(LRU));
    INIT_LIST_HEAD(&lru_head->list);
    for(int i = 0; i < PHYSICAL_NAND_NUM; i++){
        INIT_LIST_HEAD(&lru_table[i].list);
        lru_table[i].idx = i;
    }


    //create nand file
    for (idx = 0; idx < PHYSICAL_NAND_NUM; idx++)
    {
        FILE* fptr;
        snprintf(nand_name, 100, "%s/nand_%d", NAND_LOCATION, idx);
        fptr = fopen(nand_name, "w");
        if (fptr == NULL)
        {
            printf("open fail");
        }
        fclose(fptr);
    }
    return fuse_main(argc, argv, &ssd_oper, NULL);
}
