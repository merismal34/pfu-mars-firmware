#include "fw.h"
#pragma section H


void link_build_reply_22(void)
{
    link_tx_frame[0] = 0x82ee;
    util_mem_copy(link_tx_byte_2.a, settings_upper_half, 0x80);
    uart_rx_count = 0x82;
}

void link_build_reply_23(void)
{
    link_tx_frame[0] = 0x82ee;
    util_mem_copy(link_tx_byte_2.a, settings_block_0x180, 0x80);
    uart_rx_count = 0x82;
}

void link_build_reply_9(void)
{
    link_build_reply_2();
}

void link_build_memory(void)
{
    unsigned long addr;
    unsigned short len;

    len = scsi_diag_data_len.v + 3;
    if (len > 0xff)
        len = 0xff;
    link_tx_frame[0] = ((unsigned char)len << 8) | 0xe3;
    link_tx_byte_2.a[0] = 0;
    addr = (unsigned short)link_memory_addr_low | (unsigned long)link_memory_addr_high.v << 16;
    util_mem_copy(link_tx_byte_2.a + 1, (char *)addr, len - 3);
    uart_rx_count = *(unsigned char *)&link_tx_frame[0];
}

void link_build_reply_17(void)
{
    unsigned char timer_code;
    unsigned short minutes;

    link_tx_frame[0] = 0x472;
    *(unsigned char *)link_tx_byte_2.a = 0x81;
    if (power_save_seconds.v == 0) {
        timer_code = 0xf;
    } else {
        minutes = rt_divu16(power_save_seconds.v, 60);
        if (hw_hostif_flags.f.cga_test_scan) {
            if (minutes > 60)
                timer_code = 60;
            else
                timer_code = (unsigned char)minutes;
        } else {
            if (minutes > 60) {
                timer_code = rt_divu16(minutes, 5);
                timer_code |= 0x80;
            } else {
                timer_code = (unsigned char)minutes;
            }
        }
    }
    link_tx_byte_2.a[1] = timer_code;
    uart_rx_count = *(unsigned char *)&link_tx_frame[0];
}

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

void link_cmd_power(void)
{
    struct page_record *current_page;
    int saved_imask;
    unsigned short header;
    unsigned char minutes;

    current_page = &page_records[page_current.v];
    header = ((unsigned short *)scsi_param_buf)[0];
    if (header != 0x372 && (header != 0x472 || scsi_param_byte_2[0] != 0x80)) {
        link_reply_bad_frame();
        return;
    }
    if (hw_cga_board_present.f.link_scan_started && current_page->error_flags.v) {
        link_reply_busy();
        return;
    }
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (header == 0x372) {
        if (scsi_param_byte_2[0] == 0) {
            if (link_no_page_started() == 0) {
                link_reply_not_ready();
            } else if (settings_sleep_timer_default[0] == 0xfd) {
                link_reply_2();
            } else if (power_state.f.saving == 0) {
                current_page->flags.f.power_pending = 1;
                power_state.f.enter_request = 1;
                _builtin_set_imask(saved_imask);
                link_clear_reply();
            } else {
                link_reply_2();
            }
        } else if (scsi_param_byte_2[0] == 0x81) {
            link_reply_17();
        } else {
            link_reply_bad_frame();
        }
    } else {
        minutes = scsi_param_byte_3[0];
        if (settings_sleep_timer_default[0] == 0xfd) {
            link_reply_2();
            _builtin_set_imask(saved_imask);
            return;
        }
        if (minutes == 0) {
            power_save_seconds.v = 900;
        } else if (hw_hostif_flags.f.cga_test_scan) {
            if (minutes > 60)
                minutes = 60;
            power_save_seconds.v = minutes * 60;
        } else if (minutes & 0x80) {
            minutes &= 0x7f;
            if (minutes > 96)
                minutes = 96;
            else if (minutes == 0)
                minutes = 1;
            power_save_seconds.v = minutes * 300;
        } else {
            if (minutes > 60)
                minutes = 60;
            power_save_seconds.v = minutes * 60;
        }
        if (power_save_seconds.v < 0x564) {
            power_save_seconds.v -= 20;
        } else {
            power_save_seconds.v *= 0x3d9;
            power_save_seconds.v /= 1000;
        }
        tick_idle_seconds.v = 0;
        link_reply_2();
    }
    _builtin_set_imask(saved_imask);
}

int link_no_page_started(void)
{
    struct page_record *current_page;

    current_page = &page_records[page_current.v];
    if (current_page->sides[0].flags.f.requested)
        return 0;
    return 1;
}
