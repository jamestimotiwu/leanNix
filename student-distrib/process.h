#ifndef PROCESS_H
#define PROCESS_H

/* physical addresses: first program starts at 8MB, others start at 8MB+4MB*pid */
#define PROGRAM_START_ADDR 0x800000
#define MAX_PROGRAM_SIZE 0x400000
/* virtual address used by program (128MB + 0x48000) */
#define PROGRAM_VIRTUAL_START 0x08048000
/* programs stack is at the bottom of the 4MB page (128MB + 4MB) */
#define PROGRAM_VIRTUAL_STACK 0x08400000


#ifndef ASM
#include "types.h"

/* Max number of processes */
#define PROCESS_NUM 6
/* Max number of open file descriptors per program */
#define MAX_NUM_FD 8
/* Max size of argument */
#define MAX_ARGS   128
/* mask 8MB - 8KB*/
#define PCB_MASK 0x7FE000
/* PCB Functionality */

/* Basic file descriptors */
#define STDIN 0
#define STDOUT 1

#define FDFLAG_OPEN 0x1

/* jump table for file operations */
typedef int32_t (*read_op)(int32_t fd, void* buf, int32_t nbytes);
typedef int32_t (*write_op)(int32_t fd, const void* buf, int32_t nbytes);
typedef int32_t (*open_op)(const uint8_t* filename);
typedef int32_t (*close_op)(int32_t fd);

/* struct for file operations jump table */
typedef struct file_ops_ptr{
  read_op read_ptr;
  write_op write_ptr;
  open_op open_ptr;
  close_op close_ptr;
}file_ops_ptr_t;

extern file_ops_ptr_t stdin_file_ops, stdout_file_ops, rtc_file_ops, dir_file_ops, fs_file_ops;

/* struct for file descriptor */
typedef struct file_desc{
    file_ops_ptr_t *file_ops;
    int32_t inode;
    int32_t file_pos;
    int32_t flags;
}file_desc_t;

/* struct for PCB */
typedef struct PCB{
  /* file descriptor table */
  file_desc_t fd_arr[MAX_NUM_FD];
  /* unique identifier of process */
  int32_t process_id;
  /* ESP */
  int32_t stack_ptr;
  /* EBP */
  int32_t base_ptr;
  /* when child program halts, can return control to parent */
  int32_t parent_id;
  /* shell or level above */
  int32_t level;
  /* argument storage for later use */
  int8_t argument[MAX_ARGS];
}PCB_t;

PCB_t* get_PCB(); //get PCB


/* open and close a file descriptor, and check if it's open */
void set_fd_open(int32_t fd, PCB_t *pcb);
void set_fd_close(int32_t fd, PCB_t *pcb);
int32_t fd_is_open(int32_t fd, PCB_t *pcb);

extern int32_t current_pid;


int32_t process_execute(const uint8_t* command);

int32_t command_read(int8_t* command, int8_t* arg, uint32_t offset);

uint32_t get_kernel_stack(int32_t pid);
PCB_t* create_pcb(int32_t pid);

#endif

#endif

