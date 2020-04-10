#ifndef _SYSCALL_H
#define _SYSCALL_H

#define SYS_HALT 1
#define SYS_EXECUTE 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6

#define NUM_SYSCALLS 6

#ifndef ASM
/* the c code for the syscall handler */
void syscall_int();
/* syscall handler asm code */
void syscall_assembly();
#endif

/* system call halt */
int32_t halt (uint8_t status);  // sys_call_table #1

/* system call execute */ 
int32_t execute(const uint8_t* command); //sys_call_table #2

/* system call read */ 
int32_t read(int32_t fd, void* buf, int32_t nbytes);  //sys_call_table #3

/* system call write */ 
int32_t write(int32_t fd, const void* buf, int32_t nbytes);  //sys_call_table #4 

/* system call open */ 
int32_t open(const uint8_t* filename);  //sys_call_table #5

/* system call close */ 
int32_t close(int32_t fd);  //sys_call_table #6



#endif
