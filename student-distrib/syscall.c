#include "lib.h"
#include "syscall.h"

/* syscall_int()
 *   DESCRIPTION: sys call c code
 *   INPUT/OUTPUT: none
 *   SIDE EFFECTS: prints to screen
 */
void syscall_int() {
    printf("SYSTEM CALL!\n");
}



/* system call halt */
int32_t halt (uint8_t status){

  printf("syscall halt invoked\n");
   return 0; 



}

/* system call execute */ 
int32_t execute(const uint8_t* command){

printf("syscall execute invoked\n");
 return 0;


}

/* system call read */ 
int32_t read(int32_t fd, void* buf, int32_t nbytes){


 printf("syscall read invoked\n");
 return 0; 



}

/* system call write */ 
int32_t write(int32_t fd, const void* buf, int32_t nbytes){

 printf("syscall write invoked\n");
 return 0;

}

/* system call open */ 
int32_t open(const uint8_t* filename){

printf("syscall open invoked\n");
return 0; 


}

/* system call close */ 
int32_t close(int32_t fd){

printf("syscall close invoked\n");
return 0;

}