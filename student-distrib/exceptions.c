#include "exceptions.h"

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
void system_call(){
  print_exception("EXCEPTION: SYSTEM CALL! \n");
}

void print_exception(char * str){
  printf(str);
  while(1);
}
