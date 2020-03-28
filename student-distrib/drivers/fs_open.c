/* fs_open.c - Filesystem file descriptor access
 * vim:ts=4 noexpandtab
 */

#include "fs.h"
#include "../types.h"

 /* get_dir_entry
  *   DESCRIPTION: Reads directory entry given a file_name and dir_entry
  *   INPUTS:   file_name - character array of file name
  *             dir_entry - directory entry structure to return
  *   OUTPUTS: 0 if success, -1 if fail
  *   SIDE EFFECTS:
  */
int32_t get_dir_entry(const uint8_t* file_name, dir_entry_t* dir_entry) {
    /* Iterate over dir entries in boot block for matching file name */
    /* If found populate dir_entry accordingly */
    /* Return -1 if filename dne */
    return 0;
}

/* fs_open
 *   DESCRIPTION: Open file
 *   INPUTS:   file_name - character array of file name
 *   OUTPUTS: 0 if success, -1 if fail
 *   SIDE EFFECTS:
 */
int32_t fs_open(const uint8_t* file_name) {
    /* Check if file exist calling get_dir_entry */
    /* Convert file to inode */
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
