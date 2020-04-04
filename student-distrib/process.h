#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

/* Max number of open file descriptors per program */
#define MAX_NUM_FD 8

/* File operation type (pointer to a function returning int32 */
typedef int32_t (*file_op_t)();

typedef struct openfile_t {
    file_op_t *operations; /* file operations table pointer */
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
} openfile_t;

/* Program Control Block (one per program) */
typedef struct pcb_t {
    openfile_t files[MAX_NUM_FD];
} pcb_t;


#endif
