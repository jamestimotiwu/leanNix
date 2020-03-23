#ifndef RTC_H
#define RTC_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "lib.h"

#define RTC_IRQ   8
#define RTC_PORT  0x70
#define CMOS_PORT 0x71
#define REG_A     0x8A
#define REG_B     0x8B
#define REG_C     0x8C


void rtc_int();
void rtc_init();

#endif
