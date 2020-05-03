/* process.c - process and executable system calls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "drivers/fs.h"
#include "page.h"
#include "lib.h"
#include "process.h"
#include "drivers/terminal.h"
#include "drivers/rtc.h"

/* start at -1 since shell will start with pid of 0 */
int32_t current_pid = -1;

/* 8KB */
#define PROCESS_DATA_SIZE (8<<10)
/* 8MB */
#define PCB_OFFSET (8<<20)

/* get_kernel_stack
 *   DESCRIPTION: gets the top of the kernel stack for a given pid
 *   INPUTS: pid -- the pid of the process
 *   OUTPUTS: address of the stack
 *   SIDE EFFECTS: none
 */
uint32_t get_kernel_stack(int32_t pid) {
	/* get the bottom of the kernel stack */
	return PCB_OFFSET - PROCESS_DATA_SIZE*(pid);
}

PCB_t *create_pcb(int32_t pid) {
	/* PCB address: 8MB - 8KB*(pid+1) */
	PCB_t *pcb = (PCB_t *) (PCB_OFFSET - PROCESS_DATA_SIZE*(pid+1));
	return pcb;
}

/* bad_call
 *   DESCRIPTION: used instead of terminal open/close
 *   INPUTS: none
 *   OUTPUTS: -1 (failure)
 */
int32_t bad_call() {
	return -1;
}

/* get_current_terminal
 *   DESCRIPTION: gets number of terminal to be printed to
 *   INPUTS: none
 *   OUTPUTS: id of terminal
 *   SIDE EFFECTS: none
 */
uint32_t get_current_terminal() {
	/* By default, print to terminal 0 */
	uint32_t term = 0;

	if (current_pid != -1) {
		term = create_pcb(current_pid)->term_num;
	}
	return term;
}

file_ops_ptr_t stdin_file_ops = { (read_op)terminal_read, (write_op)bad_call,
	                              (open_op)bad_call, (close_op)bad_call };

file_ops_ptr_t stdout_file_ops = { (read_op)bad_call, (write_op)terminal_write,
	                               (open_op)bad_call, (close_op)bad_call };

file_ops_ptr_t rtc_file_ops = {(read_op)rtc_read, (write_op)rtc_write,
							   (open_op)rtc_open, (close_op)rtc_close };

file_ops_ptr_t dir_file_ops = {(read_op)directory_read, (write_op)directory_write,
							   (open_op)directory_open, (close_op)directory_close };

file_ops_ptr_t fs_file_ops = {(read_op)fs_read, (write_op)fs_write,
							  (open_op)fs_open, (close_op)fs_close };

/* set_fd_open
 *   DESCRIPTION: sets the flag of a file descriptor to open
 *   INPUTS: fd -- the file descriptor
 *           pcb -- program control block
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes fd_arr in pcb
 */
void set_fd_open(int32_t fd, PCB_t *pcb) {
	/* set the fd's open flag to 1 */
	pcb->fd_arr[fd].flags = FDFLAG_OPEN;
}

/* set_fd_close
 *   DESCRIPTION: sets the flag of a file descriptor to closed
 *   INPUTS: fd -- the file descriptor
 *           pcb -- program control block
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes fd_arr in pcb
 */
void set_fd_close(int32_t fd, PCB_t *pcb) {
	/* set the fd's open flag to 0 */
	pcb->fd_arr[fd].flags = FDFLAG_CLOSED;
}

int32_t fd_is_open(int32_t fd, PCB_t *pcb) {
	return (pcb->fd_arr[fd].flags == FDFLAG_OPEN);
}

