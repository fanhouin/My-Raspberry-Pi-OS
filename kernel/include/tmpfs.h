#ifndef TMPFS_H_
#define TMPFS_H_

#include <vfs.h>
#include <list.h>

typedef struct tmpfs_inode{
    struct list_head list;
    /* 
     * inode head: means the num of blocks
     * data inode: means the idx 
     */
    unsigned long idx; 
    unsigned long size;
    char *data; // data block size = MAX_DATA_LEN
    struct vnode* vnode;
}TmpfsInode;

int tmpfs_setup_mount(FileSystem *fs, Mount *mount);
Dentry *tmpfs_create_dentry(const char *name, Dentry *parent, enum dentry_type type);
VNode *tmpfs_create_vnode(Dentry *dentry);
void tmpfs_set_ops();

int tmpfs_write(struct file* file, const void* buf, size_t len);
int tmpfs_read(struct file* file, void* buf, size_t len);
int tmpfs_open(struct vnode* file_node, struct file** target);
int tmpfs_close(struct file* file);
long tmpfs_lseek64(struct file* file, long offset, int whence);

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_mkdir(struct vnode* dir_node, struct vnode** target, const char* component_name);


#endif