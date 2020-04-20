/* page.h - Defines for various page_tables, structure of page descriptor
 * and selectors
 * vim:ts=4 noexpandtab
 */
#ifndef PAGE_H
#define PAGE_H

#include "types.h"

#define NUM_PAGES       1024
#define PAGE_SIZE       4096
#define MB_PAGE_SIZE    (PAGE_SIZE << 10) 
#define VMEM_MAP        0xB8

#define KB_BASE_OFFSET  12
#define MB_BASE_OFFSET  22

/* Program image page address translation; virtual address always 0x08048000*/
#define PROC_PAGE_INDEX     32
#define PROC_PAGE_OFFSET    0x48000
#define PROC_PAGE_PADDR     (MB_PAGE_SIZE << 1) // Starting at 8MB

#define USER_VMEM_VADDR		(MB_PAGE_SIZE << 1) // Vmem mapped to virtual address at 8MB
 /*
 Base address - 31:12 physical addr of 4k-aligned tabled
 Avail - 11:9
 G - 8 global bit for TLB behavior
 PS - 7 page size (0 if 4k page directory entry, 1 if 4M page directory entry
 A - 5 accessed (set to 1 if access is made to virtual address)
 PCD - 4 page cache disabled
 PWT - 3 page write through
 U / S - 2 user / supervisor
 R / W - 1 read/write perms
 P - 0 - present flag (set to 1 during initialization)
 */
typedef union page_dir_entry_4KB {
    uint32_t val;
    struct {
        uint32_t present    : 1;
        uint32_t rw         : 1;
        uint32_t user       : 1;
        uint32_t pwt        : 1;
        uint32_t pcd        : 1;
        uint32_t accessed   : 1;
        uint32_t reserved0  : 1;
        uint32_t page_size  : 1;
        uint32_t global     : 1;
        uint32_t available  : 3;
        uint32_t base_32_12 : 20;
    } __attribute__((packed));
} page_dir_entry_4KB_t;

/*
Base address - 31:22 physical addr of 4k-aligned tabled
PAT - 12 page attribute table index set to 0
Avail - 11:9
G - 8 global bit for TLB behavior
PS - 7 page size (0 if 4k page directory entry, 1 if 4M page directory entry
D - 6 dirty
A - 5 accessed (set to 1 if access is made to virtual address)
PCD - 4 page cache disabled
PWT - 3 page write through
U / S - 2 user / supervisor
R / W - 1 read/write perms
P - 0 - present flag (set to 1 during initialization)
*/
typedef union page_dir_entry_4MB {
    uint32_t val;
    struct {
        uint32_t present    : 1;
        uint32_t rw         : 1;
        uint32_t user       : 1;
        uint32_t pwt        : 1;
        uint32_t pcd        : 1;
        uint32_t accessed   : 1;
        uint32_t dirty      : 1;
        uint32_t page_size  : 1;
        uint32_t global     : 1;
        uint32_t available  : 3;
        uint32_t pat        : 1;
        uint32_t reserved0  : 9;
        uint32_t base_32_22 : 10;
    } __attribute__((packed));
} page_dir_entry_4MB_t;

/*
Base address - 31:22 physical addr of 4k-aligned tabled
PAT - 12 page attribute table index set to 0
Avail - 11:9
G - 8 global bit for TLB behavior
PS - 7 page size (0 if 4k page directory entry, 1 if 4M page directory entry
D - 6 dirty
A - 5 accessed (set to 1 if access is made to virtual address)
PCD - 4 page cache disabled
PWT - 3 page write through
U / S - 2 user / supervisor
R / W - 1 read/write perms
P - 0 - present flag (set to 1 during initialization)
*/
typedef union page_table_entry {
    uint32_t val;
    struct {
        uint32_t present    : 1;
        uint32_t rw         : 1;
        uint32_t user       : 1;
        uint32_t pwt        : 1;
        uint32_t pcd        : 1;
        uint32_t accessed   : 1;
        uint32_t dirty      : 1;
        uint32_t pat        : 1;
        uint32_t global     : 1;
        uint32_t available  : 3;
        uint32_t base_32_12 : 20;
    } __attribute__((packed));
} page_table_entry_t;

typedef union page_dir_entry {
    page_dir_entry_4KB_t kb;
    page_dir_entry_4MB_t mb;
} page_dir_entry_t;

page_dir_entry_t page_dir[NUM_PAGES] __attribute__((aligned(PAGE_SIZE)));

page_table_entry_t page_table[NUM_PAGES] __attribute__((aligned(PAGE_SIZE)));

page_table_entry_t vmem_page_table[NUM_PAGES] __attribute__((aligned(PAGE_SIZE)));

void init_pages(void);
void page_map_user(uint32_t proc_num);

void page_map_4kb(uint32_t user, page_table_entry_t* dest_pte, uint32_t v_addr_offset, uint32_t base_addr); 
uint32_t page_map_vmem();

#endif


