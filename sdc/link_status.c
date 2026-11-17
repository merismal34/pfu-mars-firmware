#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

struct cell_hw_port_f { struct Bits b; };

void link_build_device_status(void)
{
    struct page_record *current_page;
    int x;
    unsigned short word;
    unsigned char bits;
    int saved_imask;

    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    link_tx_frame[0] = 0xe4d;
    ((unsigned short *)link_tx_byte_2.a)[0] = 0;
    bits = 0;
    if (!power_state.f.saving && mdc_mailbox.sensor_status.f.top) {
        bits |= 0x80;
    }
    link_tx_byte_4[0] = bits;
    bits = 0;
    if (power_state.f.saving) {
        if (((struct cell_hw_port_f *)hw_port_f)->b.b1 == 0) {
            bits |= 0x80;
        }
    } else {
        if (!win_params.flags.f.adf && mdc_mailbox.sensor_status.f.document_cover_open) {
            bits |= 8;
        }
        if (mdc_mailbox.sensor_status.f.adf_cover_open) {
            bits |= 0x20;
        } else {
            if (!mdc_mailbox.sensor_status.f.no_paper) {
                bits |= 0x80;
            }
            if (mdc_mailbox.sensor_status_2.f.df) {
                bits |= 0x40;
            }
            if (mdc_mailbox.sensor_status.f.pick) {
                bits |= 4;
            }
        }
        if (tick_document_cover_event) {
            bits |= 2;
            tick_document_cover_event = 0;
        }
        if (settings_intelligent_multifeed[0] & 16 && panel_multifeed_blink_state.v & 256) {
            bits &= 0x7f;
        }
    }
    if (power_state.f.saving || power_state.f.leave_request) {
        bits &= 0x82;
    }
    if (settings_test_flags[0] & 4) {
        bits &= 0x7f;
    }
    link_tx_byte_5[0] = bits;
    word = 0;
    if (power_state.f.saving) {
        word |= 0x8000;
    }
    if (panel_current.button_scan_latch) {
        word |= 0x100;
        panel_current.button_scan_latch = 0;
    }
    if (panel_current.button_send_to_latch) {
        word |= 0x400;
        panel_current.button_send_to_latch = 0;
    }
    if (panel_current.function_number == 0 && selftest_startup_done) {
        word |= 10;
    } else {
        word |= panel_current.function_number;
    }
    if (job_link_abort_flags & 0x100) {
        _builtin_set_imask(15);
        job_link_abort_flags &= 0xfeff;
        _builtin_set_imask(saved_imask);
        word |= 0x4000;
    }
    link_tx_byte_6[0] = word;
    word = job_get_last_error() & 0xff;
    if (hw_option_flags.f.b0 && link_unused_flags.b5) {
        word |= 0x8000;
    }
    if (job_double_feed_flags.b.length_check || job_double_feed_flags.b.ultrasonic_check) {
        if (current_page->multifeed_flags.f.length || current_page->multifeed_flags.f.ultrasonic) {
            word &= 0xff00;
            if (current_page->multifeed_flags.f.length) {
                word |= 0x956;
            } else {
                word |= 0x1155;
            }
        }
    }
    link_tx_byte_8[0] = word;
    link_tx_byte_10[0] = 0;
    link_tx_byte_12[0] = job_hook();
    link_tx_byte_13[0] = 0;
    uart_rx_count = *(unsigned char *)&link_tx_frame[0];
}
void link_build_reply_18(void)
{
    int x;
    int y;

    link_tx_frame[0] = 0x164f;
    util_mem_clear(((unsigned short *)link_tx_byte_2.a), 24);
    uart_rx_count = *(unsigned char *)&link_tx_frame[0];
}
