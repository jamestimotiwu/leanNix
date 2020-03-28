#ifndef RTC_H
#define RTC_H

#include "../x86_desc.h"
#include "../lib.h"
#include "../i8259.h"

#define RTC_IRQ   8
#define RTC_PORT  0x70
#define CMOS_PORT 0x71
#define REG_A     0x8A
#define REG_B     0x8B
#define REG_C     0x8C
#define SLAVE_IRQ 2


#define _2_HZ      0x0F
#define _4_HZ      0x0E
#define _8_HZ      0x0D
#define _16_HZ     0x0C
#define _32_HZ     0x0B
#define _64_HZ     0x0A
#define _128_HZ    0x09
#define _256_HZ    0x08
#define _512_HZ    0x07
#define _1024_HZ   0x06


/* RTC interrupt handler */
void rtc_int();
/* RTC initializer */
void rtc_init();
/* Sets frequency to desired hertz */
int rtc_set_freq(int freq);
/* Initialize RTC frequency to 2hz */
int rtc_open(const uint8_t* filename);
/* Does nothing unless RTC is virtualized */
int rtc_close(int32_t fd);
/* Blocks until next interrupt */
int rtc_read(int32_t fd, void* buf, int32_t nbytes);
/* Changes frequency */
int rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
