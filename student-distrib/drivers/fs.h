/* fs.h - Defines filesystem structs
 * vim:ts=4 noexpandtab
 */
#include "../types.h"
#include "../multiboot.h"

/* Directory entry and filename length limits */
#define DIR_ENTRY_LIMIT		63
#define FILENAME_CHAR_LIMIT	32

#define BLOCK_LIMIT			1023


#define DIR_ENTRY_RESERVED 24
#define BOOT_BLOCK_RESERVED 52

/* Directory entry structure */
typedef struct dir_entry {
	uint8_t filename[FILENAME_CHAR_LIMIT];
	uint32_t type;							/* 0 -> rtc, 1 -> directory, 2 -> file*/
	uint32_t inode_num;
	uint8_t reserved[DIR_ENTRY_RESERVED];
} dir_entry_t;

/*	Index node structure
	Data block holds index to data block */
typedef struct inode {
	uint32_t length;
	uint32_t data_block[BLOCK_LIMIT];
} __attribute__((packed)) node_t;

/* Boot block structure */
typedef struct boot_block {
	uint32_t dentry_count;
	uint32_t inodes_count;
	uint32_t blocks_count;
	uint8_t reserved[BOOT_BLOCK_RESERVED];
	dir_entry_t dir_entries[DIR_ENTRY_LIMIT];
} boot_block_t;

boot_block_t* fs;

void init_fs(uint32_t boot_block_addr);
int32_t get_dir_entry(const uint8_t* file_name, dir_entry_t* dir_entry);
int32_t fs_open(const uint8_t* file_name);


