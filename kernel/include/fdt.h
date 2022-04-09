#ifndef FDT_H_
#define FDT_H_
#define uint32_t unsigned int

#define FDT_MAGIC       0xd00dfeed
#define FDT_BEGIN_NODE  0x00000001
#define FDT_END_NODE    0x00000002
#define FDT_PROP        0x00000003
#define FDT_NOP         0x00000004
#define FDT_END         0x00000009

typedef struct _fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
}fdt_header;



typedef void (*dtb_callback)(char *, uint32_t, uint32_t, uint32_t*);

void fdt_traverse(fdt_header *, dtb_callback);
void initramfs_callback(char *, uint32_t, uint32_t, uint32_t*);
void show_tree_callback(char *, char *, uint32_t, uint32_t);

uint32_t big_to_little(uint32_t);


#endif

