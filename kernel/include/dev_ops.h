#ifndef DRIVER_OPS_H_
#define DRIVER_OPS_H_

#include <stddef.h>
#include <vfs.h>
#include <tmpfs.h>

void dev_ops_init();
int uart_dev_write(struct file* file, const void* buf, size_t len);
int uart_dev_read(struct file* file, void* buf, size_t len);
int framebuf_dev_write(struct file* file, const void* buf, size_t len);


#endif