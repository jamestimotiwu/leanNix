/* fs_open.c - Filesystem file descriptor access
 * vim:ts=4 noexpandtab
 */

#include "fs.h"
#include "../types.h"
#include "../lib.h"

boot_block_t *fs;

/* init_fs
 *   DESCRIPTION: Loads file system given starting point of boot block
 *   INPUTS:   boot_block_addr - address of boot block
 *   OUTPUTS: None
 *   SIDE EFFECTS:
 */
void init_fs(uint32_t boot_block_addr) {
    /* Set file system metadata block to address of boot block structure */
    fs = (boot_block_t *) boot_block_addr;
    printf("Filesystem loaded with Block count: %d, Directory entries: %d \n", fs->blocks_count, fs->dentry_count);
}

 /* get_dir_entry
  *   DESCRIPTION: Reads directory entry given a file_name and dir_entry
  *   INPUTS:   file_name - character array of file name
  *             dir_entry - directory entry structure to return
  *   OUTPUTS: 0 if success, -1 if fail
  *   SIDE EFFECTS:
  */
int32_t get_dir_entry(const uint8_t* file_name, dir_entry_t* dir_entry) {
    dir_entry_t* dir_entries;
    int i;

    /* Check if fs initialized */
    if (fs)
        dir_entries = fs->dir_entries;
    else
        /* can't proceed unless filesystem was initialized */
        return -1;
    
    /* Iterate over dir entries in boot block for matching file name */
    for (i = 0; i < fs->dentry_count; i++) {
        /* If filename match, populate dir_entry accordingly */
        if (strncmp((const int8_t*) file_name, (const int8_t*) dir_entries[i].filename, FILENAME_CHAR_LIMIT)) {
            memcpy(dir_entry, &dir_entries[i], sizeof(dir_entry));
            return 0;
        }
    }
    /* Return -1 if filename dne */
    return -1;
}

/* fs_open
 *   DESCRIPTION: Open file
 *   INPUTS:   file_name - character array of file name
 *   OUTPUTS: 0 if success, -1 if fail
 *   SIDE EFFECTS:
 */
int32_t fs_open(const uint8_t* file_name) {
    dir_entry_t* dir_entry;

    /* Validate file exist calling get_dir_entry */
    if (get_dir_entry(file_name, dir_entry) == -1)
        return -1;

    return 0;
}

/* fs_close
 *   DESCRIPTION: Close file 
 *   INPUTS:   file_name - character array of file name
 *   OUTPUTS: 0 if success, -1 if fail
 *   SIDE EFFECTS:
 */
int32_t fs_close(int32_t fd) {
    /* Close file given file descriptor */
    return 0;
}

/* temporary global var - remove it once file descriptors are implemented */
static int32_t inode_to_read = -1;
static uint32_t read_offset = 0;

/* temp_setFile
 *   DESCRIPTION: since file descriptors aren't implemented yet, use this.
 *                It sets the file to be used by fs_read
 *   INPUTS: fname -- file name
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes global var
 */
int32_t temp_setFile(char *fname) {
	dir_entry_t dentry;

	/* set dentry and make sure it is valid */
	if (read_dentry_by_name((uint8_t *)fname, &dentry) != 0)
		return -1;

	inode_to_read = dentry.inode_num;
	read_offset = 0;
	
	return 0;
}

/* fs_read
 *   DESCRIPTION: reads <count> bytes of data from file into <buf>
 *   INPUTS: fd -- file descriptor
 *           count -- number of bytes to copy
 *           buf -- the destination buffer for the data
 *   OUTPUTS: 0 if success, -1 otherwise
 *   SIDE EFFECTS: changes buf input
 */
int32_t fs_read(uint32_t fd, uint8_t *buf, uint32_t count) {

	if (inode_to_read == -1) {
		/* inode must be initialized with temp_setFile */
		return -1;
	}

	// TODO; check the type of the file (eg. FTYPE_DIR)

	int cnt = read_data(inode_to_read, read_offset, buf, count);

	/* Check if read caused error, eg. maybe a bad inode number */
	if (cnt == -1)
		return -1;

	/* Next time file is read, start at the next position to be copied */
	read_offset += cnt;

    return cnt;
}

/* fs_write
 *   DESCRIPTION: doesn't write to file system because it is read-only 
 *   INPUTS: fd -- file descriptor
 *   OUTPUTS: -1
 *   SIDE EFFECTS: none
 */
int32_t fs_write(int32_t fd) {
    /* this is a read-only file system; writing to it is an error */
    return -1;
}

/* read_dentry_by_name
 *   DESCRIPTION: fills the dir_entry based on the string file name
 *   INPUTS: fname -- the file name as a string
 *           dentry -- the directory entry to be filled out
 *   OUTPUTS: 0 if successful, -1 if not (ie. fname not in directory)
 *   SIDE EFFECTS: changes dentry
 */
int32_t read_dentry_by_name(const uint8_t *fname, dir_entry_t *dentry) {
    int i;
	/* loop through all the directory entries in boot block, searching for match */
    for (i = 0; i < fs->dentry_count; i++) {
        if (strncmp((int8_t*)fs->dir_entries[i].filename, (int8_t*)fname, FILENAME_CHAR_LIMIT) == 0) {
            
            /* A match; fill out dentry */
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
int32_t read_dentry_by_index(uint32_t index, dir_entry_t *dentry) {
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

/* read_dentry_by_name
 *   DESCRIPTION: fills the dir_entry with the name, 
 *   INPUTS: inode -- the inode
 *           offset -- offset into the file
 *           buf -- destination location of all the data
 *           length -- number of bytes to copy
 *   OUTPUTS: number of bytes copied to buf, or -1 if error
 *   SIDE EFFECTS: changes dentry
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    uint32_t i, block_index;

    node_t *node;
    uint8_t *dblock;
    
    if (inode >= fs->inodes_count)
        /* inode out of range */
        return -1;
    
    /* put address of inode struct into node */
    node = (node_t *) ((uint8_t *)fs + BLOCK_SIZE*(inode + 1));

    for (i = 0; i < length; i++) {
        int j = i+offset;

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

        dblock = (uint8_t *)fs + BLOCK_SIZE*(block_index + 1 + fs->inodes_count);

        buf[i] = dblock[j % BLOCK_SIZE];
        
    }

	/* return how many bytes were copied */
    return i;
}

