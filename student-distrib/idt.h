#ifndef IDT_H
#define IDT_H

#include "x86_desc.h"
#include "tests.h"
#include "exceptions.h"
#include "interrupt_linkage.h"

#define RESERVED_NUMS                   0x20

#define IDT_DIVIDE_ERROR                0x00
#define IDT_DEBUG                       0x01
#define IDT_NMI                         0x02
#define IDT_BREAKPOINT                  0x03
#define IDT_OVERFLOW                    0x04
#define IDT_BOUND_RANGE_EXCEEDED        0x05
#define IDT_INVALID_OPCODE              0x06
#define IDT_DEVICE_NOT_AVAILABLE        0x07
#define IDT_DOUBLE_FAULT                0x08
#define IDT_COPROCESS_SEGMENT_OVERRUN   0x09
#define IDT_INVALID_TSS                 0x0A
#define IDT_SEGMENT_NOT_PRESENT         0x0B
#define IDT_STACK_FAULT                 0x0C
#define IDT_GENERAL_PROTECTION          0x0D
#define IDT_PAGE_FAULT                  0x0E
#define IDT_FPU_FLOATING_POINT          0x10
#define IDT_ALIGNMENT_CHECK             0x11
#define IDT_MACHINE_CHECK               0x12
#define IDT_SIMD_FLOATING_POINT         0x13



#define IDT_SYSTEM_CALL                 0x80
/* IRQ1 0x20 + 1 */
#define IDT_KEYBOARD                    0x21
/* IRQ8 0x20 + 8 */
#define IDT_RTC                         0x28
/* IRQ0 0x20 + 0 */
#define IDT_PIT                         0x20



/* Initialize IDT */
extern void init_idt();

#endif
