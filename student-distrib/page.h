#include "types.h"

typedef struct page
{
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;   
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;  
    uint32_t unused     : 7; 
    uint32_t frame      : 20; 
} page_t;

typedef struct page_table
{
    page_t pages[1024];                 /* 1024 page table entries in a page table */
} page_table_t;

typedef struct page_directory
{
    page_table* tables[1024];           /* 1024 page directory entries in page directory */
    uint32_t physical_mem_table[1024];
    uint32_t physical_address;
} page_directory_t;

void switch_page_dir(page_directory_t* page_dir);

page_t* get_page(uint32_t address, int make, page_directory_t* dir);