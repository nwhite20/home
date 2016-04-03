#ifndef _RTC_H
#define _RTC_H

#include "interrupts.h"
#include "types.h"

#define RTC_IRQ 8
#define RTC_PORT 0x70
#define RTC_DATA 0x71
#define REGISTER_A	0x8A
#define REGISTER_B	0x8B
#define REGISTER_C	0x0C

/* definitions for initializing rtc */
#define TWOHZRATE   0x02
#define RTCMASK1	0x40
#define RTCMASK2	0xF0


extern void test_interrupts(void);

void rtc_init();

/* PIC INTERRUPTS */
void do_rtc();


int32_t rtc_read(int32_t fd,void * buff,int32_t num_bytes);
int32_t rtc_write(int32_t fd,int32_t buff,int32_t num_bytes);
int32_t rtc_open(const uint8_t filename);
int rtc_close();

#endif /* _RTC_H */
