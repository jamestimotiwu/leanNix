/* fs.c - Filesystem file descriptor access
 * vim:ts=4 noexpandtab
 */

#include "fs.h"
#include "rtc.h"
#include "../types.h"
#include "../lib.h"

boot_block_t* fs;

/* init_fs
 *   DESCRIPTION: Loads file system given starting point of boot block
 *   INPUTS:   boot_block_addr - address of boot block
 *   OUTPUTS: None
 *   SIDE EFFECTS:
 */
void init_fs(uint32_t boot_block_addr) {
	/* Set file system metadata block to address of boot block structure */
	fs = (boot_block_t*)boot_block_addr;
	printf("Filesystem loaded with Block count: %d, Directory entries: %d \n", fs->blocks_count, fs->dentry_count);
}

/* temporary global var - remove it once file descriptors are implemented */
static int32_t inode_to_read = -1;
static int type_to_read = -1;

/* offset into the file being read */
static uint32_t read_offset = 0;

/* fs_open
 *   DESCRIPTION: Open file
 *   INPUTS: fname -- file name
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes global var
 */
int32_t fs_open(const uint8_t* file_name) {
	dir_entry_t dentry;

	/* set dentry and make sure it is valid */
	if (read_dentry_by_name((uint8_t*)file_name, &dentry) != 0)
		return -1;

	inode_to_read = dentry.inode_num;
	read_offset = 0;
	type_to_read = dentry.type;

	if (type_to_read == FTYPE_RTC) {
		rtc_open(file_name);
	}

	return 0;
}

/* fs_close
 *   DESCRIPTION: Close file
 *   INPUTS:   file_name - character array of file name
 *   OUTPUTS: 0 if success, -1 if fail
 *   SIDE EFFECTS:
 */
int32_t fs_close(int32_t fd) {
	int retValue = 0;
	/* Close file given file descriptor */
	if (inode_to_read == -1) {
		/* fail because there is nothing to close */
		return -1;
	}
	/* must call RTC close if the filetype is RTC */
	if (type_to_read == FTYPE_RTC) {
		retValue = rtc_close(fd);
	}
	inode_to_read = -1;
	type_to_read = -1;
	read_offset = 0;
	return retValue;
}


/* read_dir_file
 *   DESCRIPTION: reads <count> bytes of file names from boot block
 *   INPUTS: offset -- how many bytes to skip over to start with
 *           count -- number of bytes to copy
 *           buf -- the destination buffer for the data
 *   OUTPUTS: number of bytes copied, or -1 if error
 *   SIDE EFFECTS: changes buf input
 */
int32_t read_dir_file(uint32_t offset, uint8_t* buf, uint32_t count) {
	int i, j;
	int dir;

	for (i = 0; i < count; i++) {
		j = i + offset;
		dir = offset / FILENAME_CHAR_LIMIT;

		if (dir >= fs->dentry_count) {
			/* No more bytes left to copy */
			return i;
		}

		/* put the correct character in the buffer */
		buf[i] = fs->dir_entries[dir].filename[j % FILENAME_CHAR_LIMIT];
	}
	return i;
}

/* fs_read
 *   DESCRIPTION: reads <count> bytes of data from file into <buf>
 *   INPUTS: fd -- file descriptor
 *           count -- number of bytes to copy
 *           buf -- the destination buffer for the data
 *   OUTPUTS: number of bytes copied, or -1 if error
 *   SIDE EFFECTS: changes buf input
 */
int32_t fs_read(uint32_t fd, uint8_t* buf, uint32_t count) {
	int cnt;

	if (inode_to_read == -1) {
		/* inode must be initialized with open */
		return -1;
	}

	if (type_to_read == FTYPE_DIR) {
		// inode_to_read is meaningless in this case

		int cnt = read_dir_file(read_offset, buf, count);
		read_offset += cnt;
		return cnt;

	}
	else if (type_to_read == FTYPE_RTC) {

		return rtc_read(fd, buf, count);

	}

	cnt = read_data(inode_to_read, read_offset, buf, count);

	/* Check if read caused error, eg. maybe a bad inode number */
	if (cnt == -1)
		return -1;

	/* Next time file is read, start at the next position to be copied */
	read_offset += cnt;

	return cnt;
}

/* fs_write
 *   DESCRIPTION: tries to write to open file (only possible for RTC file)
 *   INPUTS: fd -- file descriptor
 *           buf -- data
 *           count -- length of the data
 *   OUTPUTS: 0 if success, -1 if failure
 *   SIDE EFFECTS: none
 */
int32_t fs_write(int32_t fd, uint8_t *buf, uint32_t count) {
	if (inode_to_read == -1) {
		/* file must have been opened */
		return -1;
	}
	if (type_to_read == FTYPE_RTC) {

		return rtc_write(fd, buf, count);

	} else {
		/* this is a read-only file system; writing to it is an error */
		return -1;
	}
}

/* read_dentry_by_name
 *   DESCRIPTION: fills the dir_entry based on the string file name
 *   INPUTS: fname -- the file name as a string
 *           dentry -- the directory entry to be filled out
 *   OUTPUTS: 0 if successful, -1 if not (ie. fname not in directory)
 *   SIDE EFFECTS: changes dentry
 */
int32_t read_dentry_by_name(const uint8_t* fname, dir_entry_t* dentry) {
	int i;

	/* loop through all the directory entries in boot block, searching for match */
	for (i = 0; i < fs->dentry_count; i++) {
		/* A match; fill out dentry */
		if (strncmp((int8_t*)fs->dir_entries[i].filename, (int8_t*)fname, FILENAME_CHAR_LIMIT) == 0) {
			strncpy((int8_t*)dentry->filename, (int8_t*)fname, FILENAME_CHAR_LIMIT);
			dentry->type = fs->dir_entries[i].type;
			dentry->inode_num = fs->dir_entries[i].inode_num;
			/* return success */
			return 0;
		}
	}
	return -1;
}

/* read_dentry_by_index
 *   DESCRIPTION: fills the dir_entry based on the index into the boot block
 *   INPUTS: index -- index
 *           dentry -- the directory entry to be filled out
 *   OUTPUTS: 0 if successful, -1 if not (ie. index doesn't exist)
 *   SIDE EFFECTS: changes dentry
 */
int32_t read_dentry_by_index(uint32_t index, dir_entry_t* dentry) {
	dir_entry_t dentry_src;

	if (index >= fs->dentry_count)
		/* directory index is out of range -> error */
		return -1;

	dentry_src = fs->dir_entries[index];

	/* copy dentry_src into dentry */
	strncpy((int8_t*)dentry->filename, (int8_t*)dentry_src.filename, FILENAME_CHAR_LIMIT);
	dentry->type = dentry_src.type;
	dentry->inode_num = dentry_src.inode_num;

	/* A success */
	return 0;
}

/* read_data
 *   DESCRIPTION: reads some amount of bytes from the file
 *   INPUTS: inode -- the inode
 *           offset -- offset into the file
 *           buf -- destination location of all the data
 *           length -- number of bytes to copy
 *   OUTPUTS: number of bytes copied to buf, or -1 if error
 *   SIDE EFFECTS: changes read_offset, the buffer
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	uint32_t i, block_index;

	node_t* node;
	uint8_t* dblock;

	if (inode >= fs->inodes_count)
		/* inode out of range */
		return -1;

	/* put address of inode struct into node */
	node = (node_t*)((uint8_t*)fs + BLOCK_SIZE * (inode + 1));

	for (i = 0; i < length; i++) {
		int j = i + offset;

		if (j >= node->length) {
			/* reached the end of the file */
			/* return how many bytes were copied */
			return i;
		}

		block_index = node->data_block[j / BLOCK_SIZE];

		if (block_index >= fs->blocks_count) {
			/* Bad data block found; return error */
			return -1;
		}

		dblock = (uint8_t*)fs + BLOCK_SIZE * (block_index + 1 + fs->inodes_count);
		buf[i] = dblock[j % BLOCK_SIZE];
	}

	/* return how many bytes were copied */
	return i;
}

