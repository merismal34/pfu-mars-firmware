/* console.h: declarations of console.c. */
#ifndef CONSOLE_H
#define CONSOLE_H
#include "common.h"

/* console packet mode: the command byte (console_handle_rx, console_packet_in_cmd[0]) */
#define CONSOLE_PACKET_SET_GAINS_FRONT   0x00
#define CONSOLE_PACKET_SET_GAINS_BACK    0x01
#define CONSOLE_PACKET_SET_OFFSETS_FRONT 0x02
#define CONSOLE_PACKET_SET_OFFSETS_BACK  0x03
#define CONSOLE_PACKET_WRITE_WORD        0x10
#define CONSOLE_PACKET_WRITE_BYTE        0x11
#define CONSOLE_PACKET_WRITE_WORD_LINES  0x12
#define CONSOLE_PACKET_POWER_SAVE        0x82
#define CONSOLE_PACKET_WAKE              0x83
#define CONSOLE_PACKET_READ_WORDS        0x90
#define CONSOLE_PACKET_READ_BYTES        0x91
#define CONSOLE_PACKET_HOOK              0xC5
#define CONSOLE_PACKET_READ_REVISION     0xE0

struct console_hex_state {
    unsigned long v;
    unsigned char *p;
    unsigned char digits;
};
struct console_scan_state {
    unsigned char separator, column;
    unsigned char *p;
};
struct console_cmd {
    unsigned char id;
    char pad_01[3];
    void (*handler) (unsigned char);
    const unsigned char *name;
};

/* functions */
extern void console_handle_rx(void);
extern void console_send_next(void);
extern void console_send_byte(void);
extern void console_clear_rx_errors(void);
extern void console_hook_rx_byte(char byte);
extern void console_rx_state_0(void);
extern void console_rx_state_idle(void);
extern void console_rx_state_5(void);
extern void console_rx_state_6(void);
extern void console_rx_state_7(void);
extern void console_print_byte_at(void);
extern void console_rx_state_8(void);
extern void console_print_word_at(void);
extern void console_rx_state_9(void);
extern void console_unused_flush_tx(void);
extern void console_tx_if_done(void);
extern void console_flush_tx(void);
extern void console_reset_tx(void);
extern void console_tx_state_idle(void);
extern void console_tx_state_1(void);
extern void console_tx_state_2(void);
extern void console_tx_state_3(void);
extern void console_tx_state_4(void);
extern void console_tx_state_5(void);
extern void console_tx_state_6(void);
extern void console_tx_state_7(void);
extern void console_tx_state_8(void);
extern void console_tx_state_9(void);
extern void console_send_crlf(void);
extern void console_run_command(void);
extern void console_reply_illegal(void);
extern void console_send_prompt(void);
extern void console_cmd_s(unsigned char pos);
extern void console_cmd_d(unsigned char pos);
extern void console_cmd_e(unsigned char pos);
extern void console_cmd_ew(unsigned char pos);
extern void console_cmd_r(unsigned char pos);
extern void console_send_string(unsigned char *p);
extern unsigned char console_skip_char(struct console_scan_state *scan);
extern unsigned char console_parse_hex(struct console_hex_state *hex);
extern unsigned char console_hex_digit_value(unsigned char ch);
extern void console_format_hex16(unsigned long value, unsigned char **out);
extern void console_format_hex8(unsigned char value, unsigned char **out);
extern void console_store_byte(void);

/* constants */
extern void (*const console_rx_state_table[10]) (void);
extern void (*const console_tx_state_table[11]) (void);
extern struct console_cmd *const console_cmd_table[6];
extern const struct console_cmd console_cmd_entry_s;
extern const struct console_cmd console_cmd_entry_d;
extern const struct console_cmd console_cmd_entry_e;
extern const struct console_cmd console_cmd_entry_ew;
extern const struct console_cmd console_cmd_entry_r;
extern const unsigned char console_msg_crlf[4];
extern const unsigned char console_msg_backspace[5];
extern const unsigned char console_msg_prompt[3];
extern const unsigned char console_msg_illegal[28];
extern const unsigned char console_msg_crlf_illegal[28];
extern const unsigned char console_msg_crlf_illegal_crlf[30];
extern const unsigned char console_msg_esc_to_exit[27];
struct hw_sci_baud { unsigned char cks, brr; };
extern const struct hw_sci_baud console_sci1_baud[3];

#endif
