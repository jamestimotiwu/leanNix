#ifndef RTC_H
#define RTC_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "lib.h"

void rtc_int();
void rtc_init();

#endif
