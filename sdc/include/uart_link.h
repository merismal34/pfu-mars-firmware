/* uart_link.h: declarations of uart_link.c. */
#ifndef UART_LINK_H
#define UART_LINK_H
#include "common.h"

struct link_cmd { char code; char pad_01[3]; void (*handler) (void); };

/* functions */
extern void uart_handle_irq(void);
extern void uart_receive(void);
extern void uart_transmit(void);
extern void uart_start_frame_timeout(void);
extern void uart_frame_received(void);
extern void uart_end_receive(unsigned char byte);
extern void uart_start_timer(void);
extern void uart_set_line(void);
extern void uart_process(void);
extern void uart_process_frame(void);
extern void uart_send_reply(void);
extern void uart_handle_timeout(void);
extern void uart_clear_rts(void);
extern void uart_set_rts(void);
extern void uart_reset_state(void);
extern void uart_hook_process(void);
extern void uart_hook_rts(void);
extern void link_dispatch(void);
extern void link_hook_scan_control(void);
extern void link_cmd_start_scan(void);
extern void link_cmd_trigger(void);
extern void link_cmd_poll(void);

/* constants */
extern const unsigned short uart_timeout_table[8];
extern const unsigned short uart_divisor_table[];
extern const struct link_cmd link_cmd_table[28];
extern const char link_identity_fi4860[31];

/* variables */
extern int uart_saved_imask;

#endif
