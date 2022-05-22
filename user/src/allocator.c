#include <allocator.h>
#include <list.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <uart.h>
#include <cpio.h>

Frame *frames;
Buddy *buddy_list;
// Frame frames[FRAME_NUM];
// Buddy buddy_list[MAX_BUDDY_ORDER+1];

extern unsigned long long _start;
extern unsigned long long _end;
extern unsigned long long CPIO_BASE_START;
extern unsigned long long CPIO_BASE_END;

extern FreeChunkList *freechunk_list;

void all_allocator_init(){
    startup_alloc();
    freechunk_list_init();
    frames_init();
    memory_init();
    buddy_init();
}

void startup_alloc(){
    frames = (Frame *)simple_malloc(sizeof(Frame) * FRAME_NUM);
    buddy_list = (Buddy *)simple_malloc(sizeof(Buddy) * (MAX_BUDDY_ORDER+1));
    freechunk_list = (FreeChunkList *)simple_malloc(sizeof(FreeChunkList) * MAX_CHUNK_SIZE);

    print_string(UITOHEX, "[*] Startup Alloca -> frames addr = 0x", (unsigned long long)frames, 0);
    print_string(UITOHEX, " | buddy_list addr = 0x", (unsigned long long)buddy_list, 0);
    print_string(UITOHEX, " | freechunk_list addr = 0x", (unsigned long long)freechunk_list, 1);
}

void frames_init(){  
     for(unsigned int idx = 0; idx < FRAME_NUM; idx++){
        INIT_LIST_HEAD(&frames[idx].list);
        frames[idx].idx = idx;
        frames[idx].free = 1;
        frames[idx].order = 0;
        frames[idx].chunk_level = -1;
     }
}

void memory_init(){
    /* Spin tables for multicore boot (0x0000 - 0x1000) */
    uart_puts("[*] Memory Reserve(Spin tables) -> ");
    memory_reserve((void *)0x0, (void *)0x1000);

    /* Kernel image in the physical memory*/
    unsigned long long stack = (unsigned long long)&_start - 0x10000;
    uart_puts("[*] Memory Reserve(Kernel image) -> ");
    memory_reserve((void *)stack, (void *)&_end);

     /* Kernel image in the physical memory*/
    uart_puts("[*] Memory Reserve(Initramfs) -> ");
    memory_reserve((void *)CPIO_BASE_START, (void *)CPIO_BASE_END);

    /* Simple malloc in the physical memory*/
    uart_puts("[*] Memory Reserve(Simple malloc) -> ");
    memory_reserve((void *)SIMPLE_MALLOC_BASE_START, (void *)SIMPLE_MALLOC_BASE_END);

}

void memory_reserve(void *start, void *end){
    unsigned int start_idx = addr_to_frame_idx(start);
    unsigned int end_idx = addr_to_frame_idx(end);

    /* align end_idx */
    unsigned long long end_addr = (unsigned long long)end;
    end_idx = (end_addr % FRAME_SIZE == 0) ? end_idx : end_idx + 1;

    print_string(UITOHEX, "start addr: 0x", start_idx * FRAME_SIZE, 0);
    print_string(UITOHEX, " | end addr: 0x", end_idx * FRAME_SIZE, 1);


    for(; start_idx < end_idx; start_idx++){
        frames[start_idx].free = 0;
        frames[start_idx].order = -1;
    }
}

void buddy_init(){
    for(unsigned int i = 0; i <= MAX_BUDDY_ORDER; i++){
        INIT_LIST_HEAD(&buddy_list[i].list);
    }

    for(unsigned int idx = 0; idx < FRAME_NUM; idx++){
        /* 
         *  reserve frames  ->  don't do merge
         *  merge already   ->  don't merge 
         */
        if(frames[idx].free == 0 || frames[idx].order == -1) continue;
        Frame *target_frame = &frames[idx];
        Frame *buddy_frame  = find_buddy_frame(target_frame, target_frame->order);

        if( buddy_frame == NULL || 
            buddy_frame->free == 0 || 
            buddy_frame->order != target_frame->order){
            continue;
        }

        while(buddy_frame->free == 1 && buddy_frame->order == target_frame->order){
            if(target_frame->idx > buddy_frame->idx){
                buddy_frame->order++;
                target_frame->order = -1;
                target_frame = buddy_frame;
            }
            else{
                target_frame->order++;
                buddy_frame->order = -1;
            }

            buddy_frame = find_buddy_frame(target_frame, target_frame->order);  
            if(buddy_frame == NULL) break; 
        }
    }

    for(unsigned int idx = 0; idx < FRAME_NUM; idx++){
        if(frames[idx].free == 0 || frames[idx].order == -1) continue;
        list_add_tail(&frames[idx].list, &buddy_list[frames[idx].order].list);
    }

    // print_buddy_list();
}




void *buddy_alloc(unsigned int size){
    unsigned int use_frames = (size % FRAME_SIZE == 0) ? (size / FRAME_SIZE) : (size / FRAME_SIZE) + 1;
    int use_order = (int)log2(use_frames);
    for(unsigned int i = use_order; i <= MAX_BUDDY_ORDER; i++){
        if(!list_empty(&buddy_list[i].list)){
            Frame *alloca_frame = (Frame *)buddy_pop(&buddy_list[i], use_order);
            unsigned long long alloca_addr = alloca_frame->idx * FRAME_SIZE + BUDDY_ADDR_START;
            // print_use_frame(size, alloca_frame->idx, use_frames, use_order);
            // print_buddy_list();
            // memset((void *)alloca_addr, '\0', (1 << alloca_frame->order) * FRAME_SIZE);
            return (void *)alloca_addr;
        }
    }
    print_string(UITOHEX, "[x] Allocate Size: 0x", size, 1);
    uart_puts("[x] No enough memory!!!!\n");
    // print_string(UITOHEX, "[*] No enough memory!!!!", left_frame->idx, 0);
    return NULL;
}


void *buddy_pop(Buddy *buddy_list, int use_order){
    Frame *target_frame = (Frame *)buddy_list->list.next;
    list_del(&target_frame->list); // pop the free entry
    return release_redundant(target_frame, use_order);
}

Frame *find_buddy_frame(Frame *me, int order){
    /*
     * 0b11(left_frame->me) xor 0b10(order) = 0b01(buddy_frame)
     * 0b01(right_frame->me) xor 0b10(order) = 0b11(buddy_frame)
     */
    if(order == MAX_BUDDY_ORDER)
        return NULL;
    return &frames[me->idx ^ (1 << (unsigned int)order)];   
}

void *release_redundant(Frame *left_frame, int use_order){
    while(left_frame->order > use_order){
        int samll_order = left_frame->order - 1;
        Frame *right_frame = find_buddy_frame(left_frame, samll_order);
        right_frame->order = samll_order;
        left_frame->order = samll_order;
        list_add(&frames[right_frame->idx].list, &buddy_list[samll_order].list);
        // print_string(UITOHEX, "[-] Alloc Buddy -> Split: Left_Addr = 0x", left_frame->idx * FRAME_SIZE, 0);
        // print_string(UITOHEX, " | Right_Addr = 0x", right_frame->idx * FRAME_SIZE, 0);
        // print_string(UITOA, " | order = ", left_frame->order, 1);
    }
    left_frame->free = 0;
    return left_frame;
}

int addr_to_frame_idx(void *addr){
    unsigned long long offset = (unsigned long long)addr;
    unsigned int idx = (unsigned int)(offset / FRAME_SIZE);
    return idx;
}

void buddy_free(void *addr){
    unsigned int idx = addr_to_frame_idx(addr);
    Frame *target_frame = &frames[idx];
    Frame *buddy_frame  = find_buddy_frame(target_frame, target_frame->order);
    int first = 1;
    /* 
     * buddy_frame is not free 
     * or buddy_frame's order is not same as target_frame's order 
     * then they are not buddy, cannot merge
     */

    if(buddy_frame == NULL || buddy_frame->free == 0 || buddy_frame->order != target_frame->order){
        // print_string(UITOHEX, "[*] Free Buddy -> Free Addr: 0x", (unsigned long long)addr, 0);
        // print_string(UITOA, " | order = ", target_frame->order, 1);

        target_frame->free = 1;

        // no buddy_frame = the init status
        if(buddy_frame != NULL) 
            list_add(&frames[target_frame->idx].list, &buddy_list[target_frame->order].list);
        // print_buddy_list();
        return;
    }

    while(buddy_frame->free == 1 && buddy_frame->order == target_frame->order){
        if(first){
            // print_string(UITOHEX, "[*] Free Buddy -> Free Addr: 0x", (unsigned long long)addr, 1);
            first = 0;
        }
        /* buddy cannot be allocated, it will be merged */
        list_del(&frames[buddy_frame->idx].list);

        if(target_frame->idx > buddy_frame->idx){
            // print_string(UITOHEX, "[+] Free Buddy -> Merge: Left_Addr = 0x", buddy_frame->idx * FRAME_SIZE, 0);
            // print_string(UITOHEX, " | Right_Addr = 0x", target_frame->idx * FRAME_SIZE, 0);
            // print_string(UITOA, " | order = ", buddy_frame->order+1, 1);
            buddy_frame->order++;
            target_frame->order = -1;
            target_frame = buddy_frame;
        }
        else{
            // print_string(UITOHEX, "[+] Free Buddy -> Merge: Left_Addr = 0x", target_frame->idx * FRAME_SIZE, 0);
            // print_string(UITOHEX, " | Right_Addr = 0x", buddy_frame->idx * FRAME_SIZE, 0);
            // print_string(UITOA, " | order = ", target_frame->order+1, 1);
            target_frame->order++;
            buddy_frame->order = -1;
        }

        buddy_frame = find_buddy_frame(target_frame, target_frame->order);  
        if(buddy_frame == NULL) break; 

    }
    target_frame->free = 1;
    list_add(&frames[target_frame->idx].list, &buddy_list[target_frame->order].list);
    // print_buddy_list();
}



void print_use_frame(unsigned int size, unsigned int frame_idx, unsigned int use_frames,int use_order){
    print_string(UITOHEX, "[*] Alloc Buddy -> Allocate Size: 0x", size, 0);
    print_string(UITOHEX, " | Frame Addr: 0x", frame_idx * FRAME_SIZE, 0);
    print_string(UITOA, " | Use Order: ", use_order, 0);
    print_string(UITOA, " | Use Frames: ", use_frames, 1);
}

void print_frame_info(Frame *frame){
    print_string(UITOA, "idx: ", frame->idx, 0);
    print_string(ITOA, " - order: ", frame->order, 1);
}

void print_buddy_list(){
    struct list_head *pos;
    for(unsigned int i = 0; i <= MAX_BUDDY_ORDER; i++){
        print_string(UITOA, "", i, 0);
        uart_puts("\t:\t");
        unsigned int first = 1;
        list_for_each(pos, &buddy_list[i].list){
            Frame *tmp = (Frame *)pos;
            if(first){
                print_string(UITOHEX, "0x", tmp->idx * FRAME_SIZE, 0);
                first = 0;
            } 
            else print_string(UITOHEX, " -> 0x", tmp->idx * FRAME_SIZE, 0);
        }
        uart_puts("\n");
    }
}

void buddy_debug(){
    void *addr1 = buddy_alloc(0x2000);
    void *addr2 = buddy_alloc(0x2000);
    void *addr3 = buddy_alloc(0x2000);
    void *addr4 = buddy_alloc(0x2000);
    void *addr5 = buddy_alloc(0x1000);
    void *addr6 = buddy_alloc(0x1000);
    void *addr7 = buddy_alloc(0x1000);
    void *addr8 = buddy_alloc(0x1000);

    buddy_free(addr1);
    buddy_free(addr2);
    buddy_free(addr3);
    buddy_free(addr4);

    buddy_free(addr5);
    buddy_free(addr6);
    buddy_free(addr7);
    buddy_free(addr8);
}

