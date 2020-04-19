#include "page.h"
#include "types.h"
#include "lib.h"
#include "page_asm.h"

/* Initialize paging structures */
void init_pages()
{
	int i;

	/* Initialize all pages in page table not video memory: not present; supervisor level; rw = 1 */
	for (i = 0; i < NUM_PAGES; i++)
	{
		page_table[i].val = 2; /* Set read write flag */
	}

	/* Mark video memory present and user */
	page_table[VMEM_MAP].present = 1;
	page_table[VMEM_MAP].rw = 1;
	page_table[VMEM_MAP].base_32_12 = VMEM_MAP;

	/* First entry of page table to directory (4kb entries), page table mapped to base address */
	page_dir[0].kb.val = 0;
	page_dir[0].kb.present = 1;
	page_dir[0].kb.rw = 1;
	page_dir[0].kb.user = 1;
	page_dir[0].kb.base_32_12 = (uint32_t)page_table >> KB_BASE_OFFSET;

	/* Second page is 4MB for kernel code -> page_size = 1 */
	page_dir[1].mb.val = 0;
	page_dir[1].mb.present = 1;
	page_dir[1].mb.rw = 1;
	page_dir[1].mb.pcd = 1;
	page_dir[1].mb.page_size = 1;
	page_dir[1].mb.global = 1;
	page_dir[1].mb.base_32_22 = (uint32_t)(MB_PAGE_SIZE) >> MB_BASE_OFFSET;

	/* Create new page for 0x080 dir index -> 32 (e.g. shell) */
	page_dir[PROC_PAGE_INDEX].mb.val = 0;
	page_dir[PROC_PAGE_INDEX].mb.present = 1;
	page_dir[PROC_PAGE_INDEX].mb.rw = 1;
	page_dir[PROC_PAGE_INDEX].mb.user = 1; // User-level programs at user page
	page_dir[PROC_PAGE_INDEX].mb.pcd = 1;
	page_dir[PROC_PAGE_INDEX].mb.page_size = 1;
	page_dir[PROC_PAGE_INDEX].mb.global = 1;
	page_dir[PROC_PAGE_INDEX].mb.base_32_22 = (uint32_t)(PROC_PAGE_PADDR) >> MB_BASE_OFFSET; // Check if proc page offset is required
	/* Load and enable page directory */
	load_page(&page_dir);
}

/* page_map_user
 *   DESCRIPTION Creates page that maps to a physical address 
 *   INPUTS: proc_num -- physical memory of user program starts at 8MB + (proc_num * 4MB)
 *   OUTPUTS: -1 if error, 0 for success
 *   SIDE EFFECTS: swaps page address mapped to current program image address; flushes tlb
 */
void page_map_user(uint32_t proc_num) {
	/* Update user page physical address pointer for virtual address (physical memory of user program starts at 8MB + (proc_num * 4MB)) */
	page_dir[PROC_PAGE_INDEX].mb.base_32_22 = (uint32_t)(PROC_PAGE_PADDR + (MB_PAGE_SIZE * proc_num)) >> MB_BASE_OFFSET; // Check if proc page offset is required
	flush_tlb();

    /* Phyiscal address is located at 8MB+(pid*4MB) */
}

