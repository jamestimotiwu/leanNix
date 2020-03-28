/* fs_open.c - Filesystem file descriptor access
 * vim:ts=4 noexpandtab
 */

#include "fs.h"
#include "../types.h"
#include "../lib.h"

boot_block_t *fs;

int32_t init_fs(uint32_t boot_block_addr) {
    /* Set file system metadata block to address of boot block structure */
    fs = (boot_block_t *) boot_block_addr;
}

 /* get_dir_entry
  *   DESCRIPTION: Reads directory entry given a file_name and dir_entry
  *   INPUTS:   file_name - character array of file name
  *             dir_entry - directory entry structure to return
  *   OUTPUTS: 0 if success, -1 if fail
  *   SIDE EFFECTS:
  */
int32_t get_dir_entry(const uint8_t* file_name, dir_entry_t* dir_entry) {
    dir_entry_t* dir_entries = fs->dir_entry;
    int i;
    
    /* Iterate over dir entries in boot block for matching file name */
    for (i = 0; i < fs->dentry_count; i++) {
        /* If filename match, populate dir_entry accordingly */
        if (strncmp(file_name, dir_entries[i].filename, FILENAME_CHAR_LIMIT) {
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
    dir_entry_t* dir;

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
