#include "fw.h"
#pragma section H

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
extern int _builtin_get_cr(void);
extern void _builtin_set_cr(int);

struct cell_u8_param { unsigned char v; };


void link_clear_reply(void) { link_reply.w = 0; }
void link_reply_1(void) { link_set_reply(1); }
void link_reply_2(void) { link_set_reply(2); }
void link_reply_4(void) { link_set_reply(4); }
void link_reply_5(void) { link_set_reply(5); }
void link_reply_6(void) { link_set_reply(6); }
void link_reply_7(void) { link_set_reply(7); }
void link_reply_8(void) { link_set_reply(8); }
void link_reply_21(void) { link_set_reply(21); }
void link_reply_22(void) { link_set_reply(22); }
void link_reply_23(void) { link_set_reply(23); }
void link_reply_9(void) { link_set_reply(9); }
void link_reply_10(void) { link_set_reply(10); }
void link_reply_11(void) { link_set_reply(11); }
void link_reply_13(void) { link_set_reply(13); }
void link_reply_14(void) { link_set_reply(14); }
void link_reply_16(void) { link_set_reply(16); }
void link_reply_17(void) { link_set_reply(17); }
void link_reply_18(void) { link_set_reply(18); }
void link_reply_20(void) { link_set_reply(20); }
/* Every link command ends here: the reply code, link_reply bit 7 = reply pending. */
void link_set_reply(char code)
{
    link_reply.c.code = code;
    link_reply.b |= 128;
}

void link_reply_busy(void)
{
    struct page_record *current_page;

    current_page = &page_records[page_current.v];
    if (current_page->error_flags.c.f.hardware_error)
        link_reply_6();
    else if (current_page->error_flags.c.f.medium_error)
        link_reply_5();
    else
        link_reply_4();
}

void link_reply_bad_frame(void)
{
    err_set_illegal_request(64);
    link_reply_4();
}

void link_reply_not_ready(void)
{
    err_set_illegal_request(128);
    link_reply_4();
}

void link_cmd_nop_e8(void)
{
    link_reply_2();
}

void link_cmd_scan_control(void)
{
    unsigned char mode;
    int saved_imask;
    struct page_record *current_page;

    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    if (((unsigned short *)scsi_param_buf)[0] != 0x344) {
        link_reply_bad_frame();
        return;
    }
    mode = scsi_param_byte_2[0] & 3;
    if (mode == 3) {
        err_set_illegal_request(8);
        link_reply_4();
        return;
    }
    switch (mode) {
    case 1:
        if (link_no_page_started() == 0) {
            link_reply_not_ready();
            return;
        }
        util_mem_clear(link_window_front_params, 64);
        util_mem_clear(link_window_mode_params, 32);
        link_hook_scan_control();
        link_set_window_front();
        link_reply_1();
        break;
    case 2:
        if ((current_page->flags.v & PAGE_FLAGS_ACTIVE) == 0) {
            _builtin_set_imask(15);
            job_aborted = 1;
            job_adf_sheet_count = 0;
            job_sheet_count = 0;
            _builtin_set_cr((saved_imask & 15) << 4 | _builtin_get_cr() & ~0xf0);
            mdc_request_stop();
            link_reply_1();
            break;
        } else {
            _builtin_set_cr((saved_imask & 15) << 4 | _builtin_get_cr() & ~0xf0);
            link_reply_1();
            break;
        }
    case 0:
    default:
        link_reply_1();
        break;
        goto lbl;
    lbl:
        mdc_wake_board();
        isr_stop_cmt1();
        hw_restart_flags.v |= 128;
        rt_restart();
    }
}

void link_cmd_settings(void)
{
    int unused_frame, unused_frame_2, unused_frame_3;

    switch (scsi_param_byte_2[0]) {
    case 32:
        link_read_settings();
        break;
    case 40:
        link_save_settings();
        break;
    case 16:
        link_reply_8();
        break;
    case 17:
        link_reply_21();
        break;
    case 18:
        link_reply_22();
        break;
    case 19:
        link_reply_23();
        break;
    case 33:
        link_write_settings();
        break;
    default:
        link_reply_2();
        return;
    }
}

void link_read_settings(void)
{
    unsigned char *src;
    unsigned char *dst;
    int u;

    if (scsi_param_byte_4[0] == *scsi_param_buf - 5 && scsi_param_byte_4[0] != 0) {
        iic_eeprom_cell.v = scsi_param_byte_3[0];
        src = scsi_param_byte_5;
        dst = settings;
        if (iic_busy.b7) {
            dst = iic_pending_settings;
            iic_pending_len = scsi_param_byte_4[0];
            iic_pending_offset = scsi_param_byte_3[0];
            iic_busy.b6 = 1;
        }
        dst = dst + iic_eeprom_cell.v;
        if (iic_eeprom_cell.v + scsi_param_byte_4[0] <= 0x200)
            util_mem_copy(dst, src, scsi_param_byte_4[0]);
        else
            util_mem_copy(dst, src, -iic_eeprom_cell.v + 0x200);
        util_mem_copy_to_odd(((unsigned char *)&mdc_settings), settings, 0x100);
        if (settings_intelligent_multifeed[0] & 0x40) {
            settings_intelligent_multifeed[0] &= ~0x40;
            main_settings_request = 1;
        }
        if (((unsigned char *)&settings_setup_flags)[0] & 0x80)
            scsi_setup_mode = 1;
        else
            scsi_setup_mode = 0;
    }
    job_apply_settings();
    if (iic_busy.b7)
        link_clear_reply();
    else
        link_reply_2();
}

void link_write_settings(void)
{
    unsigned char *src;
    unsigned char *dst;
    unsigned short len;
    unsigned short u1;
    unsigned short off;
    unsigned short u2;

    len = ((struct cell_u8_param *)scsi_param_byte_6)[0].v | ((struct cell_u8_param *)scsi_param_byte_5)[0].v << 8;
    if (len == 0) {
        link_reply_2();
        return;
    }
    off = ((struct cell_u8_param *)scsi_param_byte_4)[0].v | ((struct cell_u8_param *)scsi_param_byte_3)[0].v << 8;
    if (off < 88 && off + len > 88) {
        off = 88 - off;
        src = scsi_param_byte_7;
        src = src + off;
        if (*src != 0) {
            link_reply_2();
            return;
        }
    }
    iic_eeprom_cell.v = ((struct cell_u8_param *)scsi_param_byte_4)[0].v | ((struct cell_u8_param *)scsi_param_byte_3)[0].v << 8;
    src = scsi_param_byte_7;
    dst = settings;
    if (iic_busy.b7) {
        dst = iic_pending_settings;
        iic_pending_len = len;
        iic_pending_offset = iic_eeprom_cell.v;
        iic_busy.b6 = 1;
    }
    dst = dst + iic_eeprom_cell.v;
    if (iic_eeprom_cell.v + len <= 0x200)
        util_mem_copy(dst, src, len);
    else
        util_mem_copy(dst, src, -iic_eeprom_cell.v + 0x200);
    util_mem_copy_to_odd(((unsigned char *)&mdc_settings), settings, 0x100);
    if (settings_intelligent_multifeed[0] & 0x40) {
        settings_intelligent_multifeed[0] &= ~0x40;
        main_settings_request = 1;
    }
    if (((unsigned char *)&settings_setup_flags)[0] & 0x80)
        scsi_setup_mode = 1;
    else
        scsi_setup_mode = 0;
    job_apply_settings();
    if (iic_busy.b7)
        link_clear_reply();
    else
        link_reply_2();
}

void link_save_settings(void)
{
    int u;

    if (iic_busy.b7) {
        iic_busy.b6 = 1;
        link_clear_reply();
        return;
    }
    iic_eeprom_save_settings();
    link_reply_2();
}
void link_cmd_write_memory(void)
{
    unsigned char *src;
    unsigned char *dst;
    unsigned int addr_low;
    unsigned char cnt;

    if (!(maint_mode_word.v & 0x8000)) {
        link_reply_bad_frame();
        return;
    }
    if (scsi_param_byte_2[0]) {
        if (scsi_param_byte_2[0] != 0x80) {
            link_reply_bad_frame();
            return;
        }
        if (*scsi_param_buf <= 5) {
            link_reply_2();
            return;
        }
        cnt = *scsi_param_buf - 5;
        addr_low = (unsigned short)(((struct cell_u8_param *)scsi_param_byte_4)[0].v | ((struct cell_u8_param *)scsi_param_byte_3)[0].v << 8);
        dst = (unsigned char *)((unsigned short)addr_low | (unsigned long)link_memory_addr_high.v << 16);
        src = scsi_param_byte_5;
        while (cnt > 0) {
            *dst++ = *src++;
            cnt--;
        }
    } else {
        if (((unsigned short *)scsi_param_buf)[0] == 0x54e) {
            link_reply_bad_frame();
            return;
        }
        link_memory_addr_high.v = ((struct cell_u8_param *)scsi_param_byte_4)[0].v | ((struct cell_u8_param *)scsi_param_byte_3)[0].v << 8;
    }
    link_reply_2();
}

void link_cmd_read_memory(void)
{
    if (((unsigned short *)scsi_param_buf)[0] != 0x5e5 || !(maint_mode_word.v & 0x8000)) {
        link_reply_bad_frame();
        return;
    }
    scsi_diag_data_len.v = scsi_param_byte_2[0];
    link_memory_addr_low = ((struct cell_u8_param *)scsi_param_byte_4)[0].v | ((struct cell_u8_param *)scsi_param_byte_3)[0].v << 8;
    link_reply_10();
}

void link_cmd_set_first_date(void)
{
    int saved_imask;
    int u;

    if (((unsigned short *)scsi_param_buf)[0] != 0x675 || scsi_param_byte_2[0] != 0) {
        link_reply_bad_frame();
        return;
    }
    if (settings_first_read_month[0] == 0 || settings_first_read_day[0] == 0) {
        if (job_check_date(scsi_param_byte_3[0], scsi_param_byte_4[0], scsi_param_byte_5[0]) == 0) {
            link_reply_2();
        } else {
            saved_imask = _builtin_get_imask();
            _builtin_set_imask(15);
            settings_first_read_year[0] = scsi_param_byte_3[0];
            settings_first_read_month[0] = scsi_param_byte_4[0];
            settings_first_read_day[0] = scsi_param_byte_5[0];
            _builtin_set_imask(saved_imask);
            if (iic_busy.b7) {
                iic_busy.b5 = 1;
                link_clear_reply();
            } else {
                iic_eeprom_save_settings();
                link_reply_2();
            }
        }
    } else {
        link_reply_2();
    }
}

void link_cmd_set_baud(void)
{
    int saved_imask;
    unsigned char baud_code;

    if (((unsigned short *)scsi_param_buf)[0] != 0x32d) {
        link_reply_bad_frame();
        return;
    }
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    baud_code = scsi_param_byte_2[0];
    if (baud_code >= 0x80 && baud_code <= 0x81 || baud_code >= 0x84 && baud_code <= 0x85) {
        baud_code &= 7;
        uart_baud_index_new = baud_code;
    } else if (baud_code == 0) {
        baud_code = settings_uart_baud[0] & 3;
        if (baud_code == 0)
            baud_code = 1;
        else if (baud_code == 1)
            baud_code = 0;
        uart_baud_index_new = baud_code;
    } else {
        link_reply_bad_frame();
        return;
    }
    _builtin_set_imask(saved_imask);
    link_reply_2();
}

void link_cmd_set_settings_text(void)
{
    unsigned char i;

    if (*scsi_param_buf < 5 || scsi_param_byte_3[0] != *scsi_param_buf - 4 || scsi_param_byte_3[0] > 0xc0) {
        link_reply_not_ready();
        return;
    }
    if (scsi_param_byte_2[0] == 0) {
        for (i = 0; i < 16; i++)
            settings_link_text[i] = 32;
        i = scsi_param_byte_3[0];
        if (i > 16)
            i = 16;
        util_mem_copy(settings_link_text, scsi_param_byte_4, i);
    }
    link_reply_2();
}

void link_cmd_nop_f8(void)
{
    if (((unsigned short *)scsi_param_buf)[0] != 0x2f8) {
        link_reply_bad_frame();
        return;
    }
    link_reply_2();
}

int link_build_reply_4(void)
{
    struct page_record *current_page;

    current_page = &page_records[page_current.v];
    current_page->error_flags.c.f.illegal_request = 0;
    if (current_page->illegal_request_bits) {
        current_page->illegal_request_reported.v = current_page->illegal_request_bits;
        current_page->illegal_request_bits = 0;
    }
    link_tx_byte_2.a[0] = current_page->illegal_request_reported.c.lo;
    link_tx_frame[0] = 0x355;
    uart_rx_count = 3;
}


void link_build_reply_5(void)
{
    struct page_record *current_page;

    current_page = &page_records[page_current.v];
    current_page->error_flags.c.f.medium_error = 0;
    if (current_page->medium_error_bits.v) {
        current_page->medium_error_reported.v = current_page->medium_error_bits.v;
        current_page->medium_error_bits.v = 0;
        if (current_page->medium_error_reported.v == 1 || current_page->medium_error_reported.v == 2) {
            if (power_mdc_restart_request == 0)
                mdc_request_stop();
        }
    }
    if (current_page->medium_error_reported.v & 1) {
        if (current_page->error_kind == 6 || current_page->error_kind == 7) {
            current_page->medium_error_reported.v &= 0xfe;
            current_page->medium_error_reported.v |= 0x40;
        }
    }
    link_tx_byte_2.a[0] = current_page->medium_error_reported.c.lo;
    link_tx_frame[0] = 0x354;
    uart_rx_count = 3;
}

void link_build_reply_6(void)
{
    struct page_record *current_page;

    current_page = &page_records[page_current.v];
    current_page->error_flags.c.f.hardware_error = 0;
    if (current_page->hardware_error_bits) {
        current_page->hardware_error_reported.v = current_page->hardware_error_bits;
        current_page->hardware_error_bits = 0;
    }
    link_tx_byte_2.a[0] = current_page->hardware_error_reported.c.lo;
    link_tx_frame[0] = 0x350;
    uart_rx_count = 3;
}

void link_build_reply_8(void)
{
    if (hw_hostif_flags.f.cga_test_scan) {
        link_tx_frame[0] = 0xc2ee;
        util_mem_copy(link_tx_byte_2.a, settings, 0xc0);
        if (((unsigned char *)&settings_setup_flags)[0] & 1) {
            link_tx_byte_189[0] = 0;
            link_tx_byte_185[0] = 0;
            link_tx_byte_50[0] = 0;
            link_tx_byte_51[0] = 0;
            link_tx_byte_184[0] = 0;
            link_tx_byte_130[0] = 0;
            link_tx_byte_179[0] = 0;
            link_tx_byte_180[0] = 0;
            link_tx_byte_131[0] = 0;
            link_tx_byte_43[0] = 0;
            link_tx_byte_182[0] = 0;
            link_tx_byte_120[0] = 0;
            link_tx_byte_199[0] = 0;
        }
        uart_rx_count = 0xc2;
    } else {
        link_tx_frame[0] = 0x82ee;
        util_mem_copy(link_tx_byte_2.a, settings, 0x80);
        if (((unsigned char *)&settings_setup_flags)[0] & 1) {
            link_tx_byte_50[0] = 0;
            link_tx_byte_51[0] = 0;
            link_tx_byte_43[0] = 0;
            link_tx_byte_120[0] = 0;
        }
        uart_rx_count = 0x82;
    }
}

void link_build_reply_21(void)
{
    link_tx_frame[0] = 0x82ee;
    util_mem_copy(link_tx_byte_2.a, settings_block_0x80, 0x80);
    if (((unsigned char *)&settings_setup_flags)[0] & 1) {
        link_tx_byte_82[0] = 8;
        link_tx_byte_88[0] = 0;
        link_tx_byte_89[0] = 0;
        link_tx_byte_90[0] = 0;
        link_tx_byte_91[0] = 0;
        link_tx_byte_85[0] = 0;
        link_tx_byte_84[0] = 0;
        link_tx_byte_66[0] = 0;
        link_tx_byte_61[0] = 0;
        link_tx_byte_64[0] = 0;
        link_tx_byte_77[0] = 0;
        link_tx_byte_57[0] = 0;
        link_tx_byte_72[0] = 0;
        link_tx_byte_73[0] = 0;
        link_tx_byte_56[0] = 0;
        link_tx_byte_81[0] = 0;
        link_tx_byte_2.a[0] = 0;
        link_tx_byte_51[0] = 0;
        link_tx_byte_52[0] = 0;
        link_tx_byte_2.a[1] = 0;
        link_tx_byte_54[0] = 0;
        link_tx_byte_71[0] = 0;
    }
    uart_rx_count = 0x82;
}
