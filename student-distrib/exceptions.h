#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#define EXCEPTION_STATUS 256

#ifndef ASM
#include "x86_desc.h"
#include "lib.h"

void divide_error_exception();
void debug_exception();
void nmi_interrupt();
void breakpoint_exception();
void overflow_exception();
void bound_range_exceeded_exception();
void invalid_opcode_exception();
void device_not_available_exception();
void double_fault_exception();
void coprocessor_segment_overrun_exception();
void invalid_tss_exception();
void segment_not_present_exception();
void stack_fault_exception();
void general_protection_exception();
void page_fault_exception();
void fpu_floating_point_error();
void alignment_check_exception();
void machine_check_exception();
void simd_floating_point_exception();
void reserved();
void system_call();

/* Prints the exception*/
void print_exception(char * str);
#endif

#endif
