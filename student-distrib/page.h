/* page.h - Defines for various page_tables, structure of page descriptor
 * and selectors
 * vim:ts=4 noexpandtab
 */

#include "types.h"

#define NUM_PAGES   1024
#define PAGE_SIZE   4096

typedef struct __attribute__((packed)) page_entry
{
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;   
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;  
    uint32_t unused     : 7; 
    uint32_t frame      : 20; 
} page_entry_t;

page_entry_t page_table[NUM_PAGES] __attribute__((aligned(PAGE_SIZE)));

page_entry_t page_dir[NUM_PAGES] __attribute__((aligned(PAGE_SIZE)));

page_entry_t page_vmem[NUM_PAGES] __attribute__((aligned(PAGE_SIZE)));