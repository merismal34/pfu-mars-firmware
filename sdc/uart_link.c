#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

struct uart_regs_link {
    char pad_00;
    unsigned char rbr_thr;
    char pad_02;
    union byte_bits ier;
    char pad_04[3];
    struct Bits lcr;
    char pad_08;
    struct Bits mcr;
    char pad_0a;
    struct Bits lsr;
    char pad_0c;
    struct Bits msr;
};

struct board_ctrl_reg_0c {
    char pad_00[12];
    struct {
        unsigned short p:4,
                       b11:1,
                       cga_reset:1,
                       :1,
                       cga_select:1,
                       :8;
    } reg_0c;
};


struct cell_u8_param { unsigned char v; };

/* Initialised data (section D). */

/* Uninitialised data (section B). */
int uart_saved_imask;

#pragma section H
unsigned char *link_param_dest[5] = {
    (unsigned char *)link_window_front_params,
    (unsigned char *)link_window_back_params,
    link_z_params + 16,
    link_z_params + 32,
    (unsigned char *)link_side_back_params
};
void (*link_param_apply_table[5])(void) = {
    link_set_window_front, link_set_window_back, link_apply_global_params, link_apply_side_params_front,
    link_apply_side_params_back
};
int (*link_param_x_checks[26])(int *, unsigned char *, unsigned char *, int) = {
    link_param_x_check_0, link_param_x_check_1, link_param_x_check_2, link_param_x_check_3,
    link_param_x_check_4, link_param_x_check_5, link_param_x_check_6, link_param_reject,
    link_param_x_check_8, link_param_reject, link_param_x_check_10, link_param_reject,
    link_param_reject, link_param_reject, link_param_x_check_14, link_param_reject,
    link_param_reject, link_param_reject, link_param_x_check_18, link_param_x_check_19,
    link_param_x_check_20, link_param_x_check_21, link_param_x_check_22, link_param_reject,
    link_param_reject, link_param_reject
};
int (*link_param_x80_checks[12])(int *, unsigned char *, unsigned char *, int) = {
    link_param_x80_check_0, link_param_x80_check_1, link_param_x80_check_2, link_param_x80_check_3,
    link_param_x80_check_4, link_param_x80_check_5, link_param_x80_check_6, link_param_x80_check_7,
    link_param_reject, link_param_x80_check_9, link_param_x80_check_10, link_param_x80_check_11
};
int (*link_param_y_checks[8])(int *, unsigned char *, unsigned char *, int) = {
    link_param_y_check_0, link_param_y_check_1, link_param_y_check_2, link_param_y_check_3,
    link_param_y_check_4, link_param_reject, link_param_reject, link_param_y_check_7
};
int (*link_param_z_checks[9])(int *, unsigned char *, unsigned char *, int) = {
    link_param_z_check_0, link_param_z_check_1, link_param_reject_2, link_param_reject_2,
    link_param_z_check_4, link_param_z_check_5, link_param_reject_2, link_param_reject_2,
    link_param_reject_2
};
void (*link_reply_table[24])(void) = {
    link_build_reply_none,
    link_build_reply_1,
    link_build_reply_2,
    link_build_reply_3,
    (void(*)(void))link_build_reply_4,
    link_build_reply_5,
    link_build_reply_6,
    link_build_page_info,
    link_build_reply_8,
    link_build_reply_9,
    link_build_memory,
    link_build_reply_11,
    link_build_reply_none,
    link_build_capabilities,
    link_build_identity,
    link_build_reply_none,
    link_build_device_status,
    link_build_reply_17,
    link_build_reply_18,
    link_build_reply_none,
    link_build_reply_none,
    link_build_reply_21,
    link_build_reply_22,
    link_build_reply_23
};
#pragma section

/* Constants (section C). */
#pragma section H
const unsigned short uart_timeout_table[8] = {0x71, 0x71, 0xe1, 0x1c2, 0x39, 0x39, 0x39, 0x71};
/* The UART divisor table; uart_divisor_low_bytes (read by hw_selftest.c) is a view of
   its bytes from the second on. */
const unsigned short uart_divisor_table[6] = {0x0270, 0x0138, 0x04e0, 0x09c0, 0x009c, 0x004e};
const struct link_cmd link_cmd_table[28] = {
    {0x44, {0, 0, 0}, link_cmd_scan_control}, {0x58, {0, 0, 0}, link_cmd_set_params_x}, {0x59, {0, 0, 0}, link_cmd_set_params_x}, {0x54, {0, 0, 0}, link_cmd_trigger},
    {0x53, {0, 0, 0}, link_cmd_start_scan}, {0x41, {0, 0, 0}, link_cmd_poll}, {0x42, {0, 0, 0}, link_cmd_get_status}, {0x5a, {0, 0, 0}, link_cmd_set_params_z},
    {0x43, {0, 0, 0}, link_cmd_get_page_info}, {0x4c, {0, 0, 0}, link_cmd_get_device_status}, {0x5e, {0, 0, 0}, link_cmd_set_imprinter}, {0x48, {0, 0, 0}, link_cmd_set_imprint_text},
    {0x46, {0, 0, 0}, link_cmd_set_halftone}, {0x47, {0, 0, 0}, link_cmd_set_table_data}, {0x40, {0, 0, 0}, link_cmd_get_capabilities}, {0x45, {0, 0, 0}, link_cmd_get_detected_size},
    {-24, {0, 0, 0}, link_cmd_nop_e8}, {-18, {0, 0, 0}, link_cmd_settings}, {-28, {0, 0, 0}, link_cmd_write_memory}, {-27, {0, 0, 0}, link_cmd_read_memory},
    {0x4a, {0, 0, 0}, link_cmd_get_identity}, {0x70, {0, 0, 0}, link_cmd_wake}, {0x72, {0, 0, 0}, link_cmd_power}, {0x75, {0, 0, 0}, link_cmd_set_first_date},
    {0x2d, {0, 0, 0}, link_cmd_set_baud}, {-14, {0, 0, 0}, link_cmd_set_settings_text}, {-8, {0, 0, 0}, link_cmd_nop_f8}, {0, {0, 0, 0}, 0}
};
const unsigned char link_level_table[8] = {0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xc0};
const unsigned char link_z_value_table[8] = {8, 0x50, 0x30, 0x10, 0x80, 0xa0, 0xc0, 0xff};
const unsigned short link_table_data_lengths[34] = {
    1, 4, 9, 0x10, 0x19, 0x24, 0x31, 0x40,
    0x51, 0x64, 0x79, 0x90, 0xa9, 0xc4, 0xe1, 0x100,
    0x121, 0x144, 0x169, 0x190, 0x1b9, 0x1e4, 0x211, 0x240,
    0x271, 0x2a4, 0x2d9, 0x310, 0x349, 0x384, 0x3c1, 0x400,
    0x300, 0xc00
};
const unsigned char link_param_x_check_5_table[64] = {
    0xf0, 0xf0, 0xf0, 0, 0, 0, 0, 0, 0xf0, 0xf0, 0xf0, 0xf0, 0, 0, 0xf0, 0,
    0xf0, 0xf0, 0xf0, 0xf0, 0, 0, 0xf0, 0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf0, 0, 0, 0, 0, 0, 0xf0, 0xf0, 0xf0, 0xf0, 0, 0, 0xf0, 0,
    0xf0, 0xf0, 0xf0, 0, 0, 0, 0, 0, 0xf0, 0xf0, 0xf0, 0xf0, 0, 0, 0xf0, 0
};
const char link_identity_fi4860[31] = "\x1eKFi-4860Cd     EL4J           ";
const unsigned char link_identity_fi6770[31] = "\x1eKFi-6770       0015594*       ";
const unsigned char link_identity_fi5750c[34] = "\x1eKfi-5750C      0014790*       ";
const unsigned char link_counter_format_5[8] = "05ud";
const unsigned char link_counter_format_8[5] = "08ud";
#pragma section

void uart_handle_irq(void)
{
    if (uart_irq_status[0].iir.f.f == 1) {
        uart_transmit();
    } else {
        uart_receive();
    }
}

void uart_receive(void)
{
    int x;
    unsigned short pos;
    unsigned char byte;

    uart_saved_imask = _builtin_get_imask();
    if (uart_timeout_flags.b7) {
        _builtin_set_imask(15);
        uart_timeout_flags.b6 = 1;
        _builtin_set_imask(uart_saved_imask);
    }
    uart_rx_status.w = 0;
    while (!uart_rx_status.w) {
        if (uart_irq_status[0].lsr.b.b0) {
            uart_rx_status.b.b7 = 1;
        }
        if (uart_irq_status[0].lsr.b.b1) {
            uart_rx_status.g.overrun = 1;
        }
        if (uart_irq_status[0].lsr.b.b3) {
            uart_rx_status.g.framing_error = 1;
        }
        if (uart_irq_status[0].lsr.b.b2) {
            uart_rx_status.g.parity_error = 1;
        }
    }
    uart_rx_status.b.b7 = 0;
    isr_stop_cmt1();
    _builtin_set_imask(15);
    if (!((struct uart_regs_link *)p_uart[0])->mcr.b1) {
        ((struct uart_regs_link *)p_uart[0])->mcr.b1 = 1;
    }
    _builtin_set_imask(uart_saved_imask);
    uart_rx_count = 0;
    byte = ((struct uart_regs_link *)p_uart[0])->rbr_thr;
    if (uart_irq_status[0].lsr.b.b0) {
        uart_irq_status[0].lsr.b.b0 = 0;
    }
    if (uart_timeout_mode.g.ack_timeout) {
        uart_end_receive(byte);
    } else {
        pos = uart_frame_len.v;
        if (pos < 0x207) {
        } else if (pos > 0xc06 || ((unsigned short *)scsi_param_buf)[0] != 0x547) {
            uart_rx_status.g.frame_overflow = 1;
            pos = 23;
        }
        ((char *)scsi_param_buf)[pos] = byte;
        uart_frame_len.v++;
        if (uart_rx_status.w) {
            uart_start_frame_timeout();
        } else if (((unsigned short *)scsi_param_buf)[0] == 0x547) {
            pos = (((struct cell_u8_param *)scsi_param_byte_4)[0].v | ((struct cell_u8_param *)scsi_param_byte_3)[0].v << 8) + 5;
            if (pos == uart_frame_len.v) {
                uart_frame_received();
            } else {
                uart_start_frame_timeout();
            }
        } else if (((unsigned short *)scsi_param_buf)[0] == 0x7ee && uart_frame_len.v >= 7) {
            pos = (((struct cell_u8_param *)scsi_param_byte_6)[0].v | ((struct cell_u8_param *)scsi_param_byte_5)[0].v << 8) + 7;
            if (pos == uart_frame_len.v) {
                uart_frame_received();
            } else {
                uart_start_frame_timeout();
            }
        } else if (uart_frame_len.v > 0xff) {
            uart_start_frame_timeout();
        } else if (((char *)scsi_param_buf)[0] == uart_frame_len.c.lo) {
            uart_frame_received();
        } else {
            uart_start_frame_timeout();
        }
    }
}

void uart_transmit(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (uart_timeout_flags.b7) {
        _builtin_set_imask(15);
        uart_timeout_flags.b6 = 1;
        _builtin_set_imask(saved_imask);
    }
    if (uart_rx_count != 0) {
        _builtin_set_imask(15);
        ((struct uart_regs_link *)p_uart[0])->rbr_thr = *uart_tx_ptr++;
        _builtin_set_imask(saved_imask);
        uart_rx_count--;
        return;
    }
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->ier.b.b1 = 0;
    _builtin_set_imask(saved_imask);
    uart_timeout_retries = 0;
    _builtin_set_imask(15);
    uart_start_timer();
    p_cmt1->CMCSR.BIT.CMIE = 1;
    uart_timeout_mode.v = 0;
    uart_timeout_mode.g.ack_timeout = 1;
    _builtin_set_imask(saved_imask);
}

void uart_start_frame_timeout(void)
{
    _builtin_set_imask(15);
    p_cmt1->CMCSR.BIT.CMIE = 0;
    p_cmt1->CMCOR = 0x1d5;
    p_cmt->CMSTR.BIT.STR1 = 1;
    p_cmt1->CMCSR.BIT.CMIE = 1;
    uart_timeout_mode.v = 0;
    uart_timeout_mode.g.frame_timeout = 1;
    _builtin_set_imask(uart_saved_imask);
}

void uart_frame_received(void)
{
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->ier.b.b0 = 0;
    uart_pending_events.b1 = 1;
    _builtin_set_imask(uart_saved_imask);
    isr_trigger_mtu0(2);
}

void uart_end_receive(unsigned char byte)
{
    uart_timeout_mode.g.ack_timeout = 0;
    uart_frame_len.v = 0;
    if (uart_rx_status.w != 0 || byte != 0xff) {
        uart_rx_status.w = 0;
        if (uart_rx_retries >= 2) {
            link_reply.w = 0;
            uart_rx_retries = 0;
            return;
        }
        uart_rx_retries++;
        link_reply.b |= 0x80;
        isr_request_command_layer(2);
    } else {
        link_reply.w = 0;
        uart_rx_retries = 0;
        if (uart_baud_index.v != uart_baud_index_new) {
            uart_set_line();
            _builtin_set_imask(uart_saved_imask);
        }
        _builtin_set_imask(15);
        if (hw_hostif_flags.f.selected) {
            win_params.side[0].size_rounding.v = 0x6002;
            win_params.b13.f.b0 = 0;
            hw_hostif_flags.f.owner_decided = 1;
            hw_hostif_flags.f.cga_test_scan = 0;
            hw_cga_board_present.f.present = 0;
            p_intc->IPRA.WORD &= 0xf0ff;
            if (hw_board_variant == 0) {
                ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.b11 = 1;
            }
            ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_reset = 1;
            ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_select = 0;
        }
        _builtin_set_imask(uart_saved_imask);
    }
}

void uart_start_timer(void)
{
    p_cmt1->CMCSR.BIT.CMIE = 0;
    p_cmt1->CMCOR = uart_timeout_table[uart_baud_index.v * 2];
    p_cmt->CMSTR.BIT.STR1 = 1;
}

/* 16550 line set-up: LCR (+7) bit 7 = DLAB, divisor latch +1/+3 from the baud index uart_baud_index_new,
   then DLAB off and MCR (+9) bit 0 = DTR. */
void uart_set_line(void)
{
    unsigned char baud_index;

    _builtin_set_imask(15);
    baud_index = uart_baud_index_new;
    uart_baud_index.v = baud_index;
    ((struct uart_regs_link *)p_uart[0])->mcr.b0 = 0;
    ((struct uart_regs_link *)p_uart[0])->lcr.b7 = 1;
    baud_index = uart_baud_index.v;
    ((struct uart_regs_link *)p_uart[0])->rbr_thr = uart_divisor_low_bytes[baud_index * 2];
    ((struct uart_regs_link *)p_uart[0])->ier.v = uart_divisor_table[baud_index] >> 8;
    ((struct uart_regs_link *)p_uart[0])->lcr.b7 = 0;
    ((struct uart_regs_link *)p_uart[0])->mcr.b0 = 1;
}

void uart_process(void)
{
    int saved_imask;
    int x;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->ier.b.b0 = 0;
    ((struct uart_regs_link *)p_uart[0])->ier.b.b1 = 0;
    _builtin_set_imask(saved_imask);
    if (uart_pending_events.b1) {
        uart_process_frame();
    }
    if (uart_pending_events.b0) {
        uart_send_reply();
    }
    if (uart_pending_events.b2) {
        uart_handle_timeout();
    }
    _builtin_set_imask(15);
    uart_hook_process();
    uart_hook_rts();
    ((struct uart_regs_link *)p_uart[0])->ier.b.b0 = 1;
    _builtin_set_imask(saved_imask);
}

void uart_process_frame(void)
{
    int saved_imask;
    int x;

    uart_pending_events.b1 = 0;
    isr_stop_cmt1();
    uart_clear_rts();
    for (;;) {
        if ((settings_emulation[0] & 0x40) || ((struct uart_regs_link *)p_uart[0])->msr.b4) {
            break;
        }
        uart_hook_rts();
        if (!((struct uart_regs_link *)p_uart[0])->msr.b5) {
            uart_reset_state();
            uart_set_rts();
            return;
        }
    }
    rt_delay_24k();
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->ier.b.b1 = 0;
    uart_irq_status[0].lsr.b.b6 = 0;
    ((struct uart_regs_link *)p_uart[0])->rbr_thr = 0xff;
    _builtin_set_imask(saved_imask);
    for (;;) {
        if (((struct uart_regs_link *)p_uart[0])->lsr.b5) {
            break;
        }
    }
    uart_set_rts();
    link_dispatch();
    uart_frame_len.v = 0;
    if (link_reply.f.b7) {
        uart_send_reply();
    }
}

void uart_send_reply(void)
{
    int saved_imask;
    int x;

    uart_pending_events.b0 = 0;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (!link_reply.f.b7) {
        _builtin_set_imask(saved_imask);
        return;
    }
    _builtin_set_imask(saved_imask);
    link_reply.f.b7 = 0;
    if (link_reply.c.code == 0) {
        uart_rx_count = 0;
        uart_timeout_retries = 0;
        uart_set_rts();
        return;
    }
    uart_clear_rts();
    link_build_reply();
    for (;;) {
        if ((settings_emulation[0] & 0x40) || ((struct uart_regs_link *)p_uart[0])->msr.b4) {
            break;
        }
        uart_hook_rts();
        if (!((struct uart_regs_link *)p_uart[0])->msr.b5) {
            uart_reset_state();
            uart_set_rts();
            return;
        }
    }
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->ier.b.b1 = 1;
    uart_irq_status[0].lsr.b.b6 = 0;
    _builtin_set_imask(saved_imask);
}

void uart_handle_timeout(void)
{
    int saved_imask;
    unsigned short n;
    unsigned char error_byte;

    saved_imask = _builtin_get_imask();
    uart_pending_events.b2 = 0;
    isr_stop_cmt1();
    uart_frame_len.v = 0;
    for (;;) {
        if ((settings_emulation[0] & 0x40) || ((struct uart_regs_link *)p_uart[0])->msr.b4) {
            break;
        }
        uart_hook_rts();
        if (!((struct uart_regs_link *)p_uart[0])->msr.b5) {
            uart_reset_state();
            uart_set_rts();
            uart_rx_status.w = 0;
            return;
        }
    }
    if (uart_rx_status.g.parity_error) {
        error_byte = 0x80;
    } else if (uart_rx_status.g.framing_error || uart_rx_status.g.overrun) {
        error_byte = 0x81;
    } else {
        error_byte = 0x82;
    }
    uart_rx_status.w = 0;
    uart_clear_rts();
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->ier.b.b1 = 0;
    uart_irq_status[0].lsr.b.b6 = 0;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->rbr_thr = error_byte;
    _builtin_set_imask(saved_imask);
    for (;;) {
        if (((struct uart_regs_link *)p_uart[0])->lsr.b5) {
            break;
        }
    }
    _builtin_set_imask(15);
    uart_start_timer();
    _builtin_set_imask(saved_imask);
    for (;;) {
        if (p_cmt1->CMCSR.BIT.CMF) {
            _builtin_set_imask(15);
            p_cmt1->CMCSR.BIT.CMF = 0;
            _builtin_set_imask(saved_imask);
            break;
        }
    }
    isr_stop_cmt1();
    uart_set_rts();
    uart_rx_status.w = 0;
    uart_frame_len.v = 0;
}

/* +9 is the MCR, bit 1 RTS (uart_set_rts sets it). */
void uart_clear_rts(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->mcr.b1 = 0;
    _builtin_set_imask(saved_imask);
}

void uart_set_rts(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    ((struct uart_regs_link *)p_uart[0])->mcr.b1 = 1;
    _builtin_set_imask(saved_imask);
}

void uart_reset_state(void)
{
    uart_rx_count = 0;
    uart_frame_len.v = 0;
    link_reply.w = 0;
    uart_rx_retries = 0;
    uart_timeout_retries = 0;
}

void uart_hook_process(void)
{
}

void uart_hook_rts(void)
{
}

/* A frame is byte 0 = length (2 + data bytes), byte 1 = command, then the data; the command
   selects the handler in link_cmd_table. */
void link_dispatch(void)
{
    const struct link_cmd *entry;

    if (uart_frame_len.v < 2) {
        link_reply_bad_frame();
        return;
    }
    for (entry = link_cmd_table; entry->handler != 0; entry++) {
        if (entry->code == ((char *)scsi_param_byte_1)[0]) {
            break;
        }
    }
    if (entry->handler != 0) {
        entry->handler();
    } else {
        link_reply_bad_frame();
    }
}

void link_hook_scan_control(void)
{
}

void link_cmd_start_scan(void)
{
    struct page_record *current_page;
    int saved_imask;

    current_page = &page_records[page_current.v];
    if (((unsigned short *)scsi_param_buf)[0] != 0x253) {
        link_reply_bad_frame();
        return;
    }
    hw_cga_board_present.f.link_scan_started = 1;
    if (current_page->error_flags.v) {
        link_reply_busy();
        return;
    }
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.selected) {
        _builtin_set_imask(saved_imask);
        link_reply_2();
        return;
    }
    if (!hw_hostif_flags.f.owner_decided) {
        isr_reset_hostif();
        *(volatile unsigned char *)&hw_hostif_flags |= 0x80;
    }
    if (current_page->flags.f.sheet_fed) {
        _builtin_set_imask(saved_imask);
        link_reply_2();
        return;
    }
    if (settings_byte_0x9a[0] & 1) {
        current_page->flags.f.sheet_fed = 1;
        _builtin_set_imask(saved_imask);
        link_reply_2();
        return;
    }
    current_page->flags.f.load_pending = 1;
    current_page->flags.f.adf = 1;
    scsi_setup_mode = 0;
    if (win_sides.f.selection == 2 || win_sides.f.selection == 1) {
        link_start_scan_flag.b7 = 0;
    } else {
        link_start_scan_flag.b7 = 0;
    }
    _builtin_set_imask(saved_imask);
    link_clear_reply();
}

void link_cmd_trigger(void)
{
    struct page_record *current_page;
    int saved_imask;
    unsigned short word;

    current_page = &page_records[page_current.v];
    word = ((unsigned short *)scsi_param_buf)[0];
    if (word != 0x254) {
        link_reply_bad_frame();
        return;
    }
    if (current_page->error_flags.v) {
        link_reply_busy();
        return;
    }
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (!current_page->flags.f.sheet_fed) {
        _builtin_set_imask(saved_imask);
        link_reply_not_ready();
        return;
    }
    current_page->flags.f.read_pending = 1;
    word = link_global_params_ptr->side_flags.side_selection;
    if (word != 1) {
        current_page->sides[0].flags.f.requested = 1;
    } else {
        current_page->sides[1].flags.f.requested = 1;
    }
    _builtin_set_imask(saved_imask);
    link_clear_reply();
}

void link_cmd_poll(void)
{
    struct page_record *current_page;
    int saved_imask;

    current_page = &page_records[page_current.v];
    if (((unsigned short *)scsi_param_buf)[0] != 0x241) {
        link_reply_bad_frame();
        return;
    }
    if (hw_cga_board_present.f.link_scan_started && current_page->error_flags.v) {
        link_reply_busy();
        return;
    }
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (err_display_flags.b.b5) {
        _builtin_set_imask(saved_imask);
        link_reply_6();
    } else if (err_display_flags.b.b6) {
        _builtin_set_imask(saved_imask);
        link_reply_5();
    } else if (settings_intelligent_multifeed[0] & 16 && panel_multifeed_blink_state.v & 256) {
        _builtin_set_imask(saved_imask);
        link_reply_5();
    } else if (err_display_flags.b.b7) {
        _builtin_set_imask(saved_imask);
        link_reply_5();
    } else {
        _builtin_set_imask(saved_imask);
        link_reply_2();
    }
}
