#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_
#include <list.h>
#include <math.h>

#define BUDDY_ADDR_START    0x10000000
#define BUDDY_ADDR_END      0x20000000
#define FRAME_SIZE          4096
#define FRAME_NUM           ((0x20000000-0x10000000) / FRAME_SIZE)
#define MAX_BUDDY_ORDER     16      //log2(65536)

typedef struct _Buddy {
    struct list_head list;
    unsigned int free_num;
}Buddy;

typedef struct _Frame {
    struct list_head list;
    int order;
    unsigned int idx;
    unsigned int free;
}Frame;

void allocator_init();
void buddy_push(Frame *, Buddy *);
void *buddy_pop(Buddy *, int);
void *buddy_alloc(unsigned int);
void *release_redundant(Frame *, int);
void buddy_free(void *);
Frame *find_buddy_frame(Frame*, int);


void print_frame_info(Frame *);
void print_use_frame(unsigned int, unsigned int, unsigned int, int);
void print_frame_info(Frame *);
void print_buddy_list();



#endif