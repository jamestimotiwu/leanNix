#include "lib.h"
#include "syscall.h"
#include "drivers/fs.h"
#include "page.h"
#include "interrupt_linkage.h"
#include "process.h"
#include "x86_desc.h"
#include "drivers/keyboard.h"

/* halt32
 *   DESCRIPTION: same as halt, but argument is 32 bits so
 *                that this can be called during an exception
 *                (status = 256)
 *   INPUTS: status -- 32 bit status of halt
 *   OUTPUT: 0
 *   SIDE EFFECTS: halts execution of program
 */
int32_t halt32(uint32_t status) {
    PCB_t *pcb = create_pcb(current_pid);
    PCB_t *parent;
    int32_t i, ebp;

    if (current_pid == 0) {
        /* restart shell when it tries to halt */
        current_pid = -1;
        execute((uint8_t *)"shell");
        return -1;
    }

    /* restore parent data */
    current_pid = pcb->parent_id;
    /* change kernel stack to parent's kernel stack */
    parent = create_pcb(current_pid);
    tss.esp0 = parent->stack_ptr;

    /* restore parent paging */
    page_map_user(current_pid);

    /* close any relevant FDs */
    // TODO: call the correct close() function on these?
    for (i = 0; i < MAX_NUM_FD; i++) {
        set_fd_close(i, pcb);
    }

    ebp = pcb->base_ptr;

    /* jump to execute return */
    halt_ret(ebp, status);

    return 0;
}

/* halt
 *   DESCRIPTION: syscall that halts and returns execution to parent process
 *   INPUTS: status -- status of the halt
 *   OUTPUTS: none, because it doesn't return
 *   SIDE EFFECTS: ends execution of a program
 */
int32_t halt (uint8_t status){
    return halt32((uint32_t) status);
}


/* execute
 *   DESCRIPTION: syscall that executes a command
 *   INPUTS: command - the file to execute
 *   OUTPUTS: 0 if success, else -1
 *   SIDE EFFECTS: starts executing a different program
 */
int32_t execute(const uint8_t* command){
    /* Maximum 3 shells */
    if(current_pid == 2)
        return -1;
    uint32_t entry;
    PCB_t *pcb = create_pcb(current_pid + 1);
    int32_t ebp;
    int32_t parent_pid = current_pid;
    uint8_t program[KB_BUF_SIZE+1];
    int i = 0, j = 0;

    if (command == NULL)
        return -1;

    /* parse args */

    /* skip the leading whitespace */
    while (command[i] == ' ')
        i++;

    /* Copy program name from the command into kernel memory */
    while (command[i] != ' ' && command[i] != '\0') {
        program[j] = command[i];
        j++;
        i++;
    }
    program[j] = '\0';

    /* skip the whitespace */
    while (command[i] == ' ')
        i++;

    /* Copy the remaining characters in command into the program's argument */
    j = 0;
    while (command[i] != '\0') {
        pcb->argument[j] = command[i];
        j++;
        i++;
    }
    pcb->argument[j] = '\0';

    /* check file validity */
    if (!program_valid(program))
        return -1;

    /* set up paging */
    current_pid++;
    page_map_user(current_pid);

    /* load file into memory and get entry point*/
    entry = program_load(program, current_pid);

    /* create PCB/open FDs */
    //pcb = create_pcb(current_pid);
    pcb->process_id = current_pid;

    /* get the stack pointer and base pointer */
    asm volatile("movl %%ebp, %0"
            : "=rm"((ebp)) /* outputs */
            :
            : "memory");

    pcb->parent_id = parent_pid;

    pcb->base_ptr = ebp;
    pcb->stack_ptr = get_kernel_stack(current_pid);
    tss.esp0 = pcb->stack_ptr; /* set the kernel's stack pointer */
    
    /* initailize stdin and stdout */
    /* other entries (eg. inode) aren't used */
    pcb->fd_arr[STDIN].file_ops = &stdin_file_ops;
    set_fd_open(STDIN, pcb);
    pcb->fd_arr[STDOUT].file_ops = &stdout_file_ops;
    set_fd_open(STDOUT, pcb);

    /* prepare for context switch */

    /* push IRET context onto stack and do IRET */
    ret_to_user(entry);

    return 0;
}

/* read
 *   DESCRIPTION: read bytes of a file
 *   INPUTS: fd -- the file descriptor
 *           buf -- the destination buffer
 *           nbytes -- number of bytes to read
 *   OUTPUTS: number of bytes copied
 *   SIDE EFFECTS: changes buf
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);

    if (buf == NULL || fd < 0 || fd >= MAX_NUM_FD)
        return -1;

    if (fd_is_open(fd, pcb))
        return pcb->fd_arr[fd].file_ops->read_ptr(fd, buf, nbytes);

    return -1; /* file was not opened */
}

/* write
 *   DESCRIPTION: write bytes into file
 *   INPUTS: fd -- the file descriptor
 *           buf -- the source buffer
 *           nbytes -- number of bytes to write
 *   OUTPUTS: number of bytes copied
 *   SIDE EFFECTS: writes to a file
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);

    if (buf == NULL || fd < 0 || fd >= MAX_NUM_FD)
        return -1;
    if (fd_is_open(fd, pcb))
        return pcb->fd_arr[fd].file_ops->write_ptr(fd, buf, nbytes);

    return -1; /* file was not opened */
}

/* open
 *   DESCRIPTION: open a file descriptor
 *   INPUTS: filename -- the name of the file to open
 *   OUTPUTS: the file descriptor, or -1 if failure
 *   SIDE EFFECTS: changes pcb open files
 */
int32_t open(const uint8_t* filename){
    //int32_t fd;
    /* first find if there is an open file descriptor */
    //fd = 0;
    PCB_t* pcb = create_pcb(current_pid);
    int i;

    if (filename == NULL)
        return -1;

    for(i = 0; i < MAX_NUM_FD; i++){
      if(pcb->fd_arr[i].flags == 0)
        break;
    }
    if(i == MAX_NUM_FD)
      return -1;

    dir_entry_t dentry;
    if (-1 == read_dentry_by_name(filename, &dentry))
        return -1;

    //check type to assign file_ops
    if(dentry.type == FTYPE_RTC)
      pcb->fd_arr[i].file_ops = &rtc_file_ops;
    else if(dentry.type == FTYPE_DIR)
      pcb->fd_arr[i].file_ops = &dir_file_ops;
    else if(dentry.type == FTYPE_FILE)
      pcb->fd_arr[i].file_ops = &fs_file_ops;
    else
        return -1;

    pcb->fd_arr[i].inode = dentry.inode_num;
    pcb->fd_arr[i].file_pos = 0;
    pcb->fd_arr[i].file_ops->open_ptr(filename); // return this?
    pcb->fd_arr[i].flags = FDFLAG_OPEN;

    return i;
}

/* close
 *   DESCRIPTION: close a file descriptor
 *   INPUTS: filename -- the name of the file to close
 *   OUTPUTS: the file descriptor
 *   SIDE EFFECTS: changes pcb fd_arr
 */
int32_t close(int32_t fd){
    /* check that fd is valid (can't close stdout or stdin) */
    if (fd < 0 || fd >= MAX_NUM_FD || fd == STDOUT || fd == STDIN)
        return -1;

    PCB_t* pcb = create_pcb(current_pid);
    if (!fd_is_open(fd, pcb)) {
        /* trying to open an already closed fd */
        return -1;
    }

    pcb->fd_arr[fd].file_ops = &dir_file_ops;
    pcb->fd_arr[fd].flags = 0;

    return pcb->fd_arr[fd].file_ops->close_ptr(fd);
}

/* getargs
 *   DESCRIPTION: get the arguments of the command
 *   INPUTS: buf -- location to copy the argument into
 *           nbytes -- number of bytes to copy
 *   OUTPUTS: 0 on success, else -1
 *   SIDE EFFECTS: changes user-level buf
 */
int32_t getargs(uint8_t *buf, int32_t nbytes) {
    PCB_t* pcb = create_pcb(current_pid);
    int i;

    if (buf == NULL)
        return -1;

    for (i = 0; i < nbytes && pcb->argument[i] != '\0'; i++) {
        buf[i] = pcb->argument[i];
    }
    buf[i] = '\0';

    if (i == nbytes)
        /* There are still more bytes to copy */
        return -1;

    return 0;

}

/* vidmap
 *   DESCRIPTION: Sets screen address pointer to user video memory page
 *   INPUTS: screen_start - screen address pointer
 *   OUTPUTS: screen_address to user video memory page
 *   SIDE EFFECTS: modify given screen_start pointer
 */
int32_t vidmap(uint8_t** screen_start) {
    /* Validate screen_start double ptr */
    if ((uint32_t)screen_start < (MB_PAGE_SIZE * 32) || (uint32_t)screen_start >= (MB_PAGE_SIZE * 33) - 4) {
        return -1;
    }

    page_map_vmem();
    *screen_start = (uint8_t*)USER_VMEM_VADDR;
    return 0;
}

