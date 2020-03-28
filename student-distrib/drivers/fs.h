/* fs.h - Defines filesystem structs
 * vim:ts=4 noexpandtab
 */
#include "../types.h"
#include "../multiboot.h"

#define DIR_ENTRY_LIMIT		63

#define FILENAME_CHAR_LIMIT	32

/* 63 directory entries */
typedef struct boot_block {
	uint32_t dentry_count;
	uint32_t inodes_count;
	uint32_t blocks_count;
	uint8_t reserved[52];
	dir_entry_t dir_entry[DIR_ENTRY_LIMIT];
} boot_block_t;

typedef struct dir_entry {
	uint8_t filename[FILENAME_CHAR_LIMIT];
	uint32_t type;
	uint32_t inode_num;
	uint8_t reserved[24];
} dir_entry_t;