/* button.h: declarations of button.c. */
#ifndef BUTTON_H
#define BUTTON_H
#include "common.h"

/* the bits of the button state byte (button_poll) */
#define BUTTON_STATE_POWER    0x08
#define BUTTON_STATE_FUNCTION 0x10
#define BUTTON_STATE_SEND_TO  0x40
#define BUTTON_STATE_SCAN     0x80
#define BUTTON_STATE_ANY      0xF0
/* button_poll's poll counts */
#define BUTTON_DEBOUNCE_POLLS      6
#define BUTTON_LATCH_POLLS         8
#define BUTTON_UNLATCH_POLLS       0x177
#define BUTTON_POWER_OFF_POLLS     250
#define BUTTON_FUNCTION_HOLD_POLLS 124

/* functions */
extern void button_poll(void);
extern void button_handle_in_scan(void);

#endif
