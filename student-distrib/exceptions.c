#include "exceptions.h"
#include "process.h"
#include "syscall.h"

/* Exception Handlers
 *   DESCRIPTION: prints error msg and halts execution when exception is raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: prints characters, execution stops
 */
void divide_error_exception(){
    print_exception("EXCEPTION: DIVIDE_ERROR_EXCEPTION! \n");
}
void debug_exception(){
  print_exception("EXCEPTION: DEBUG_EXCEPTION! \n");
}
void nmi_interrupt(){
  print_exception("EXCEPTION: NMI_INTERRUPT! \n");
}
void breakpoint_exception(){
  print_exception("EXCEPTION: BREAKPOINT_EXCEPTION! \n");
}
void overflow_exception(){
  print_exception("EXCEPTION: OVERFLOW_EXCEPTION! \n");
}
void bound_range_exceeded_exception(){
  print_exception("EXCEPTION: BOUND_RANGE_EXCEEDED_EXCEPTION! \n");
}
void invalid_opcode_exception(){
    print_exception("EXCEPTION: INVALID_OPCODE_EXCEPTION! \n");
}
void device_not_available_exception(){
  print_exception("EXCEPTION: DEVICE_NOT_AVAILABLE_EXCEPTION! \n");
}
void double_fault_exception(){
  print_exception("EXCEPTION: DOUBLE_FAULT_EXCEPTION! \n");
}
void coprocessor_segment_overrun_exception(){
  print_exception("EXCEPTION: COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION! \n");
}
void invalid_tss_exception(){
  print_exception("EXCEPTION: INVALID_TSS_EXCEPTION! \n");
}
void segment_not_present_exception(){
  print_exception("EXCEPTION: SEGMENT_NOT_PRESENT_EXCEPTION! \n");
}
void stack_fault_exception(){
  print_exception("EXCEPTION: STACK_FAULT_EXCEPTION! \n");
}
void general_protection_exception(){
    print_exception("EXCEPTION: GENERAL_PROTECTION_EXCEPTION! \n");
}
void page_fault_exception(){
    print_exception("EXCEPTION: PAGE_FAULT_EXCEPTION! \n");
}
void fpu_floating_point_error(){
  print_exception("EXCEPTION: FPU_FLOATING_POINT_EXCEPTION! \n");
}
void alignment_check_exception(){
  print_exception("EXCEPTION: ALIGNMENT_CHECK_EXCEPTION! \n");
}
void machine_check_exception(){
  print_exception("EXCEPTION: MACHINE_CHECK_EXCEPTION! \n");
}
void simd_floating_point_exception(){
  print_exception("EXCEPTION: SIMD_FLOATING_POINT_EXCEPTION! \n");
}
void reserved(){
  print_exception("EXCEPTION: RESERVED! \n");
}

/* helper function used by the above functions */
void print_exception(char * str){
    /* Data segment register is used to determine if exception was caused in
     * user space or kernel space. */
    int16_t ds_reg;
    asm volatile ("movw %%ds, %0"
            : "=g"((ds_reg))
            :
            : "memory");

    if (ds_reg == USER_DS) {
        /* Exception caused in user space -- print message and halt */
        printf(str);
        halt32(EXCEPTION_STATUS);

    } else {
        /* Exception caused in kernel space -- print error and loop */
        /* ds == KERNEL_DS */
        clear();
        printf(str);
        while(1);
    }
}
