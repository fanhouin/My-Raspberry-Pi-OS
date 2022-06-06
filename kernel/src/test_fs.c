#include <test_fs.h>
#include <vfs.h>
#include <tmpfs.h>
#include <uart.h>
#include <string.h>
#include <user_syscall.h>

void fs_test1(){
    uart_puts("----------------------------TEST VFS_OPEN---------------------------\n");
    File *file_a = NULL;
    int err = vfs_open("hello", 0, &file_a);
    if(err) uart_puts("[x] Failed to open hello\n");

    err = vfs_open("hello", O_CREAT, &file_a);
    if(err) uart_puts("[x] Failed to open file_a 2\n");
    vfs_close(file_a);

    File *file_b = NULL;
    err = vfs_open("hello", 0, &file_b);
    if(err) uart_puts("[x] Failed to open file_b 1\n");
    vfs_close(file_b);
}

void fs_test2(){
    uart_puts("----------------------TEST VFS_WRITE & VFS_READ---------------------\n");
    File *a = NULL;
    int err = vfs_open("hello", O_CREAT, &a);
    if(err) uart_puts("[x] Failed to open \"hello\"\n");
    // uart_puts(a->vnode->dentry->parent->name);

    File *b = NULL;
    err = vfs_open("world", O_CREAT, &b);
    if(err) uart_puts("[x] Failed to open \"world\"\n");

    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World\n", 6);
    // len = vfs_write(a, "Hello ", 6);
    // if(len == -1) uart_puts("[x] Failed to write \"Hello\"\n");

    vfs_close(a);
    vfs_close(b);

    err = vfs_open("/hello", O_CREAT, &b);
    if(err) uart_puts("[x] Failed to open \"hello\"\n");
    err = vfs_open("/world", O_CREAT, &a);
    if(err) uart_puts("[x] Failed to open \"world\"\n");
    
    char buf[300];
    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_puts(buf);
    // printf("%s\n", buf); // should be Hello World!
}

void fs_test3(){
    uart_puts("-----------------------TEST VFS_MKDIR & VFS_LS----------------------\n");
    vfs_mkdir("/path1");
    int err = vfs_mkdir("/path1/path2/");
    if(err){
        switch (err)
        {
        case -2:
            uart_puts("[x] \"path1\" is exist\n");
            break;
        default:
            uart_puts("[x] Failed to mkdir \"path1\"\n");
            break;
        }
    }
    vfs_ls(NULL);
    vfs_ls("./path1/");
    vfs_ls("path1/path2");
    vfs_ls("path1/path2/../../");
    // err = vfs_ls("path1/abc");
    // if(err) uart_puts("[x] Failed to ls \"path1/abc\"\n");
}

void fs_test4(){
    uart_puts("---------------------TEST VFS_MOUNT & VFS_UMONT---------------------\n");
    vfs_chdir(NULL);
    vfs_mkdir("mnt");
    vfs_mount("mnt", "abc");
    vfs_ls(NULL);
    vfs_chdir("mnt");
    File *file_a = NULL;
    vfs_open("mount_file", O_CREAT, &file_a);
    vfs_write(file_a, "Hello Mount\n", 12);
    vfs_close(file_a);

    vfs_ls(NULL);
    vfs_chdir("../");
    vfs_ls(NULL);

    vfs_umount("mnt");
    vfs_ls(NULL);

    vfs_mkdir("/path1/path2/fanfan");
    vfs_mount("/path1/path2/fanfan", "abc");
    vfs_ls("/path1/path2/fanfan");

    vfs_open("/path1/path2/fanfan/mount_file", 0, &file_a);
    char buf[300];
    int sz;
    sz = vfs_read(file_a, buf, 100);
    buf[sz] = '\0';
    vfs_close(file_a);
    uart_puts(buf);

    // vfs_umount("/path1/path2/fanfan");

    // vfs_chdir(NULL);

}

void fs_test5(){
    int a = open("hello", O_CREAT);
    int b = open("world", O_CREAT);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("hello", 0);
    a = open("world", 0);
    print_string(ITOA, "b_fd = ", b, 0);
    print_string(ITOA, " | a_fd = ", a, 1);

    char buf[300];
    int sz;
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_puts(buf); // should be Hello World!
}

void fs_test6(){
    char buf[8];
    mkdir("mnt", 0);
    int fd = open("/mnt/a.txt", O_CREAT);
    write(fd, "Hi", 2);
    close(fd);
    chdir("mnt");
    fd = open("./a.txt", 0);
    // assert(fd >= 0);
    read(fd, buf, 2);
    uart_puts(buf);
    uart_puts("\n");
    // assert(strncmp(buf, "Hi", 2) == 0);

    chdir("..");
    mount(NULL, "mnt", "abc", 0, NULL);
    fd = open("mnt/a.txt", 0);
    // assert(fd < 0);
    if(fd < 0) uart_puts("[x] fd fail!!\n");

    vfs_umount("/mnt");
    fd = open("/mnt/a.txt", 0);
    if(fd < 0) uart_puts("[x] fd fail!!\n");

    char buf2[8];
    // assert(fd >= 0);
    read(fd, buf2, 2);
    uart_puts(buf2);
    uart_puts("\n");
    // assert(strncmp(buf, "Hi", 2) == 0);
}

void fs_test7(){
    char buf[16];
    mkdir("proc", 0);
    mount(NULL, "proc", "procfs", 0, NULL);
    int fd = open("/proc/switch", 0);
    write(fd, "0", 1);
    close(fd);

    fd = open("/proc/hello", 0);
    int sz = read(fd, buf, 16);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\n");
    // printf("%s\n", buf); // should be hello
    close(fd);

    fd = open("/proc/switch", 0);
    write(fd, "1", 1);
    close(fd);

    fd = open("/proc/hello", 0);
    sz = read(fd, buf, 16);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\n");
    // printf("%s\n", buf); //should be HELLO
    close(fd);

    fd = open("/proc/1/status", 0); // choose a created task's id here
    sz = read(fd, buf, 16);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\n");
    // printf("%s\n", buf); // status of the task.
    close(fd);

    fd = open("/proc/999/status", 0); // choose a non-existed task's id here
    if(fd < 0) uart_puts("[x] fd fail!");
    // assert(fd < 0);
}

void user_basic2(){
    mkdir("/tmp", 0);
    int fd = open("/tmp/tmpfile", O_CREAT);
    write(fd, "Hello World!", 12);
    close(fd);
    fd = open("/tmp/tmpfile", 0);
    char buf[100];
    int sz = read(fd, buf, 100);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\n");
    close(fd);

    mount(NULL, "/tmp", "tmpfs", 0, NULL);
    fd = open("/tmp/tmpfile", O_CREAT);
    write(fd, "Hello World!", 12);
    close(fd);
    fd = open("/tmp/tmpfile", 0);
    sz = read(fd, buf, 100);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\n");
    close(fd);
    
}

void user_advance1(){
    write(1, "Hello World!\n", 13);
    write(1, "JJ\n", 3);

}