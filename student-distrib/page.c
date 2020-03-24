#include "page.h"
#include "types.h"
#include "lib.h"
#include "page_asm.h"

/* Initialize paging structures */
void init_pages()
{
    int i;
    /* Enable read writes for paging */
    for (i = 0; i < NUM_PAGES; i++) {
        page_dir[0].kb.val = 2;
    }

    // Initialize all pages not video memory to not present
    for (i = 0; i < NUM_PAGES; i++)
    {
        /*
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
        */
        page_table[i].present = 0;
        page_table[i].rw = 1;
        page_table[i].user = 0;
        page_table[i].pwt = 0;
        page_table[i].pcd = 0;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].pat = 0;
        page_table[i].global = 0;
        page_table[i].available = 0;
        page_table[i].base_32_12 = 0;
    }

    // Mark video memory present and user
    page_table[VMEM_MAP].present = 1;
    page_table[VMEM_MAP].rw = 1;
    page_table[VMEM_MAP].base_32_12 = VMEM_MAP;
    //page_table[VMEM_MAP].user = 1;
    // First entry of page table to directory (4kb entries)
    /*
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
    */
    page_dir[0].kb.present = 1;
    page_dir[0].kb.rw = 1;
    page_dir[0].kb.user = 1;
    page_dir[0].kb.pwt = 0;
    page_dir[0].kb.pcd = 0;
    page_dir[0].kb.accessed = 0;
    page_dir[0].kb.reserved0 = 0;
    page_dir[0].kb.page_size = 0;
    page_dir[0].kb.global = 0;
    page_dir[0].kb.available = 0;
    page_dir[0].kb.base_32_12 = (uint32_t)page_table >> KB_BASE_OFFSET;

    // Second page is 4MB -> size = 1
    /*
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
        uint32_t base_32_22 : 10;*/

    page_dir[1].mb.present = 1;
    page_dir[1].mb.rw = 1;
    page_dir[1].mb.user = 0;
    page_dir[1].mb.pwt = 0;
    page_dir[1].mb.pcd = 1;
    page_dir[1].mb.accessed = 0;
    page_dir[1].mb.dirty = 0;
    page_dir[1].mb.reserved0 = 0;
    page_dir[1].mb.page_size = 1;
    page_dir[1].mb.global = 1;
    page_dir[1].mb.available = 0;
    page_dir[1].mb.base_32_22 = (uint32_t)(MB_PAGE_SIZE) >> MB_BASE_OFFSET;

    // Load page directory
    /* 1. Load cr3 with address of page dir */
    /* 2. Enable PSE page size extension bit of cr4*/
    /* 3. Set PG paging flag bit of cr0*/

    load_page(&page_dir);

    /*asm volatile("                       \n\
        movl %0, %%eax                   \n\
        movl %%eax, %%cr3                \n\
        movl %%cr4, %%eax                \n\
        orl $0x00000010, %%eax           \n\
        movl %%eax, %%cr4                \n\
        movl %%cr0, %%eax                \n\
        orl $0x80000000, %%eax           \n\
        movl %%eax, %%cr0                \n\
        "
        :
    : "r"(&page_dir)
        : "eax", "memory", "cc"
        );*/

}
