/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0

#ifndef ASM

/* Max number of open file descriptors per program */
#define MAX_NUM_FD 8
#define MAX_ARGS   128

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

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
}file_ops_ptr_t

/* struct for file descriptor */
typedef struct file_desc{
    file_ops_ptr_t *file_ops;
    int32_t inode;
    int32_t file_pos;
    int32_t flags;
}file_desc_t

/* struct for PCB */
typedef struct PCB{
  /* file descriptor table */
  file_desc_t fd_arr[MAX_NUM_FD];
  /* unique identifier of process */
  int32_t process_id;
  /* ESP */
  int32_t stack_ptr;
  /* when child program halts, can return control to parent */
  int32_t parent_id;
  /* shell or level above */
  int32_t level;
  /* argument storage for later use */
  int8_t arguments[MAX_ARGS];
}PCB_t;










#endif /* ASM */

#endif /* _TYPES_H */
