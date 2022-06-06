#include <dev_ops.h>
#include <uart.h>
#include <read.h>
#include <mailbox.h>
#include <malloc.h>
#include <tmpfs.h>

struct file_operations* uart_file_ops;
struct file_operations* framebuffer_file_ops;

extern unsigned int width, height, pitch, isrgb; /* dimensions and channel order */
extern unsigned char *lfb;                       /* raw frame buffer address */

void dev_ops_init(){
    uart_file_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
    uart_file_ops->read = uart_dev_read;
    uart_file_ops->write = uart_dev_write;
    uart_file_ops->open = tmpfs_open;
    uart_file_ops->lseek64 = tmpfs_lseek64;
    uart_file_ops->close = tmpfs_close;

    framebuffer_file_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
    framebuffer_file_ops->write = framebuf_dev_write;
    framebuffer_file_ops->open = tmpfs_open;
    framebuffer_file_ops->lseek64 = tmpfs_lseek64;
    framebuffer_file_ops->close = tmpfs_close;
}



int uart_dev_read(struct file* file, void* buf, size_t len){
    return readnbyte((char *)buf, len);
}

int uart_dev_write(struct file* file, const void* buf, size_t len){
    uart_nbyte((char *)buf, len);
    return len;
}

int framebuf_dev_write(struct file* file, const void* buf, size_t len){
    size_t pos = file->f_pos;
    memcpy((char *)lfb + pos, buf, len);
    file->f_pos += len;
    return len;
}
