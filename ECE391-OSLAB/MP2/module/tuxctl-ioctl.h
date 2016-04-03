// All necessary declarations for the Tux Controller driver must be in this file

#ifndef TUXCTL_H
#define TUXCTL_H

#define TUX_SET_LED _IOR('E', 0x10, unsigned long)
#define TUX_READ_LED _IOW('E', 0x11, unsigned long*)
#define TUX_BUTTONS _IOW('E', 0x12, unsigned long*)
#define TUX_INIT _IO('E', 0x13)
#define TUX_LED_REQUEST _IO('E', 0x14)
#define TUX_LED_ACK _IO('E', 0x15)

#endif

#define TUX_R 0x80
#define TUX_L 0x40
#define TUX_D 0x20
#define TUX_U 0x10
#define TUX_C 0x08
#define TUX_B 0x04
#define TUX_A 0x02
#define TUX_S 0x01
