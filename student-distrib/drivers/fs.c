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

/* fs_read
 *   DESCRIPTION: reads <count> bytes of data from file into <buf>
 *   INPUTS: fd -- file descriptor
 *           count -- number of bytes to copy
 *           buf -- the destination buffer for the data
 *   OUTPUTS: 0 if success, -1 otherwise
 *   SIDE EFFECTS: changes buf input
 */
int32_t fs_read(int32_t fd) {
    // uses read_data
    return 0;
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

int strncmp(char *s1, char *s2, int len) {
    return -1; // TODO
}

void strncpy(char *dest, char *src, int len) {
    // TODO
}

/* read_dentry_by_name
 *   DESCRIPTION: fills the dir_entry based on 
 *   INPUTS: fname -- the file name as a string
 *           dentry -- the directory entry to be filled out
 *   OUTPUTS: 0 if successful, -1 if not (ie. fname not in directory)
 *   SIDE EFFECTS: changes dentry
 */
int32_t read_dentry_by_name(const uint8_t *fname, dir_entry *dentry) {
    int i;
    for (i == 0; i < fs.dentry_count; i++) {
        if (strcmp(fs.dir_entries[i].fname, fname, FILENAME_CHAR_LIMIT) == 0) {
            
            /* A match; fill out dentry */
            strncpy(dentry.fname, fname, FILENAME_CHAR_LIMIT);
            dentry.type = fs.dir_entries[i].type;
            dentry.inode_num = fs.dir_entries[i].inode_num;
            /* return success */
            return 0;
        }

    }
    return -1;
}

/* read_dentry_by_index
 *   DESCRIPTION: fills the dir_entry based on the
 *   INPUTS: fname -- the file name as a string
 *           dentry -- the directory entry to be filled out
 *   OUTPUTS: 0 if successful, -1 if not (ie. index doesn't exist)
 *   SIDE EFFECTS: changes dentry
 */
int32_t read_dentry_by_index(uint32_t index, dir_entry *dentry) {
    return -1;
}

/* read_dentry_by_name
 *   DESCRIPTION: fills the dir_entry with the name, 
 *   INPUTS: inode -- the inode
 *           offset -- offset into the file
 *           buf -- destination location of all the data
 *           length -- number of bytes to copy
 *   OUTPUTS: filled out in dentry
 *   SIDE EFFECTS: changes dentry
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    return -1;
}

