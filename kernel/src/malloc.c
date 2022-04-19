#include <uart.h>
#include <string.h>
#include <allocator.h>
#include <malloc.h>

extern Frame *frames;
extern Buddy *buddy_list;
FreeChunkList *freechunk_list;
// FreeChunkList freechunk_list[MAX_CHUNK_SIZE];

/* define 11 level common chunk size */
unsigned int chunk_size[] = {0x10, 0x20, 0x30, 0x40, 0x60, 0x80, 
                            0xa0, 0x100, 0x200, 0x400, 0x800};

unsigned int find_level(unsigned int size){
    int i;
    for(i = 0; i < MAX_CHUNK_SIZE; i++){
        if(size <= chunk_size[i]) break;
    }
    return i;
}
void freechunk_list_init(){
    for(int i = 0; i < MAX_CHUNK_SIZE; i++){
        INIT_LIST_HEAD(&freechunk_list[i].list);
    }
}

void *kmalloc(unsigned int size){
    void *addr;

    if(size <= FRAME_SIZE / 2) 
        addr = chunk_alloc(size);
    else 
        addr = buddy_alloc(size);

    return addr;
}

void kfree(void *addr){
    int idx = addr_to_frame_idx(addr);
    if(idx == -1){
        print_string(UITOHEX, "[x] kfree error -> the addr: 0x", (unsigned long long)addr, 0);
        uart_puts(" is illegal allocated memory!!!\n");
        return;
    }
    Frame *target_frame = &frames[idx];
    if(target_frame->chunk_level >= 0)
        chunk_free(addr);
    else
        buddy_free(addr);
}


void *chunk_alloc(unsigned int size){
    if(size >= FRAME_SIZE){
        uart_puts("[x] Alloc Chunk Error -> allocate size is over 4096KB, please use \"buddy_alloc\" function\n");
        return NULL;
    } 
    unsigned int level = find_level(size);
    
    if(list_empty(&freechunk_list[level].list)){
        void *addr = buddy_alloc(0x1000);
        int idx = addr_to_frame_idx(addr);
        Frame *target_frame = &frames[idx];
        target_frame->chunk_level = (int)level;

        /* split the frame 
         * and add the chunk info(list_head) in the physical memory
         */
        unsigned int offset = 0;
        while(offset < FRAME_SIZE){
            Chunk *chunk = (Chunk *)((char *)addr + offset);
            list_add_tail(&chunk->list, &freechunk_list[level].list);
            offset += chunk_size[level];
        }
    }

    FreeChunkList *chunk_list = &freechunk_list[level];
    Chunk *chunk = (Chunk *)chunk_list->list.next;
    list_del(&chunk->list);
    print_string(UITOHEX, "[*] Alloc Chunk -> Allocate Size: ", size, 0);
    print_string(UITOHEX, " | Chunk Size: ", chunk_size[level], 0);
    print_string(UITOA, " | Level: ", level, 1);
    // print_freechunk_list();
    return (void *)chunk;
}


void chunk_free(void *addr){
    int idx = addr_to_frame_idx(addr);
    if(idx == -1){
        print_string(UITOHEX, "[x] Free Chunk error -> the addr: 0x", (unsigned long long)addr, 0);
        uart_puts(" is illegal allocated memory!!!\n");
        return;
    }
    Frame *target_frame = &frames[idx];

    if(target_frame->chunk_level == -1){
        print_string(UITOHEX, "[x] Free Chunk error -> the addr: 0x", (unsigned long long)addr, 0);
        uart_puts(" is not a chunk, please use \"buddy_free\" function\n");
        return;
    }

    /* add the chunk info(list_head) in the physical memory */
    Chunk *chunk = (Chunk *)addr;
    list_add(&chunk->list, &freechunk_list[target_frame->chunk_level].list);

    print_string(UITOHEX, "[*] Free Chunk -> the addr: 0x", (unsigned long long)addr, 1);
    // print_freechunk_list();
    
}


void print_freechunk_list(){
    struct list_head *pos;
    for(unsigned int i = 0; i < MAX_CHUNK_SIZE; i++){
        print_string(UITOHEX, "0x", chunk_size[i], 0);
        uart_puts("\t:\t");
        unsigned int first = 1;
        list_for_each(pos, &freechunk_list[i].list){
            void *tmp = (Chunk *)pos;
            if(first){
                print_string(UITOHEX, "0x", (unsigned long long)tmp, 0);
                first = 0;
            } 
            else print_string(UITOHEX, " -> 0x", (unsigned long long)tmp, 0);
        }
        uart_puts("\n");
    }
}




void chunk_debug(){
    // void *addr1 = chunk_alloc(0x2000);
    // void *addr2 = chunk_alloc(0x200);
    // void *addr3 = chunk_alloc(0x200);
    // chunk_free(addr2);
}

void kmalloc_debug(){
    // test buddy alloc & free
    uart_puts("\n--------------------------------------TEST BUDDY ALLOC & FREE--------------------------------------\n\n");
    unsigned long long size1[] = {0x900, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
                                    0x1500, 0x4000, 0x8000, 0x10000, 0x20000, 0x20001};
    void *addr1[sizeof(size1) / sizeof(unsigned long long)];

    for(int i = 0; i < sizeof(size1) / sizeof(unsigned long long); i++){
        addr1[i] = kmalloc(size1[i]);
    }
    print_buddy_list();


    for(int i = 0; i < sizeof(size1) / sizeof(unsigned long long); i++){
        kfree(addr1[i]);
    }
    print_buddy_list();

    // test buddy merge & split
    uart_puts("\n--------------------------------------TEST BUDDY MERGE & SPLIT--------------------------------------\n\n");
    void *addr3[30];
    for(int i = 0; i < 30 ; i++){
        addr3[i] = kmalloc(0x1000);  
    } 
    for(int i = 0; i < 30 ;i++){
        kfree(addr3[i]);
    }
    print_buddy_list();

    // test chunk alloc & free
    uart_puts("\n--------------------------------------TEST CHUNK ALLOC & FREE--------------------------------------\n\n");
    void *addr2[sizeof(chunk_size) / sizeof(unsigned int)];
    for(int i = 0; i < sizeof(chunk_size)/ sizeof(unsigned int); i++){
        addr2[i] = kmalloc(chunk_size[i]);
    }
    print_freechunk_list();

    for(int i = 0; i < sizeof(chunk_size) / sizeof(unsigned int); i++){
        kfree(addr2[i]);
    }
    print_freechunk_list();



}

void *simple_malloc(unsigned long size) {
    static void *head = (void *)SIMPLE_MALLOC_BASE_START;
    void *ptr = head;
    head = head + size;
    return ptr;
}


