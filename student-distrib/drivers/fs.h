/* fs.h - Defines filesystem structs
 * vim:ts=4 noexpandtab
 */
#include "../types.h"
#include "../multiboot.h"

/* Directory entry and filename length limits */
#define DIR_ENTRY_LIMIT		63
#define FILENAME_CHAR_LIMIT	32

#define BLOCK_LIMIT			1023

#define BLOCK_SIZE          (4*1024)

#define DIR_ENTRY_RESERVED  24
#define BOOT_BLOCK_RESERVED 52

#define FTYPE_RTC  0
#define FTYPE_DIR  1
#define FTYPE_FILE 2

/* Directory entry structure -- 64B */
typedef struct dir_entry {
	uint8_t filename[FILENAME_CHAR_LIMIT];
	uint32_t type;							/* 0 -> rtc, 1 -> directory, 2 -> file*/
	uint32_t inode_num;
	uint8_t reserved[DIR_ENTRY_RESERVED];
} __attribute__((packed)) dir_entry_t;

/*	Index node structure
	Data block holds index to data block -- 4KB */
typedef struct inode {
	uint32_t length;
	uint32_t data_block[BLOCK_LIMIT];
} __attribute__((packed)) node_t;

/* Boot block structure -- 64B */
typedef struct boot_block {
	uint32_t dentry_count;
	uint32_t inodes_count;
	uint32_t blocks_count;
	uint8_t reserved[BOOT_BLOCK_RESERVED];
	dir_entry_t dir_entries[DIR_ENTRY_LIMIT];
} __attribute__((packed)) boot_block_t;

boot_block_t* fs;

/* initialize file system (must be called before using any other fs functions) */
void init_fs(uint32_t boot_block_addr);

int32_t get_dir_entry(const uint8_t* file_name, dir_entry_t* dir_entry);

int32_t read_dentry_by_name(const uint8_t *fname, dir_entry *dentry);
int32_t read_dentry_by_index(uint32_t index, dir_entry *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);

/* used by system calls */
int32_t fs_open(const uint8_t* file_name);
int32_t fs_close(int32_t fd);
int32_t fs_read(int32_t fd); // TODO change params
int32_t fs_write(int32_t fd);


