#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_
#include <list.h>
#include <math.h>

#define BUDDY_ADDR_START    0x00000000
#define BUDDY_ADDR_END      0x3C000000
// #define BUDDY_ADDR_START    0x10000000
// #define BUDDY_ADDR_END      0x20000000
#define FRAME_SIZE          4096
#define FRAME_NUM           ((BUDDY_ADDR_END-BUDDY_ADDR_START) / FRAME_SIZE)
#define MAX_BUDDY_ORDER     15

typedef struct _Buddy {
    struct list_head list;
}Buddy;

typedef struct _Frame {
    struct list_head list;
    int order;
    unsigned int idx;
    unsigned int free;

    // chunk allocator  
    int chunk_level;
}Frame;

void startup_alloc();
void all_allocator_init();
void frames_init();
void memory_init();
void memory_reserve(void *start, void *end);
void allocator_init();
void buddy_init();
void buddy_push(Frame *, Buddy *);
void *buddy_pop(Buddy *, int);
void *buddy_alloc(unsigned int);
void *release_redundant(Frame *, int);
void buddy_free(void *);
Frame *find_buddy_frame(Frame*, int);
int addr_to_frame_idx(void *);


void print_frame_info(Frame *);
void print_use_frame(unsigned int, unsigned int, unsigned int, int);
void print_frame_info(Frame *);
void print_buddy_list();

void buddy_debug();

#endif