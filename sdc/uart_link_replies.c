#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
struct link_side_view_replies {
    char pad_00[2];
    struct {
        unsigned char :1,
                      color_mode:2,
                      dropout_color:3,
                      c:2;
    } color_bits;
    union {
        unsigned char b;
        struct { unsigned char top:1, rest:7; } f;
    } gamma_flags;
    char pad_04[2];
    unsigned char color_contrast, color_brightness;
};


void link_apply_side_params_front(void)
{
    link_apply_side_params(0);
}

void link_apply_side_params_back(void)
{
    link_apply_side_params(1);
}

void link_apply_side_params(int side)
{
    struct link_side_view_replies *link_side;
    int saved_imask;
    int x;
    struct win_side *window;
    unsigned char value;

    link_side = link_side_params_ptrs[side];
    saved_imask = _builtin_get_imask();
    window = &win_params.side[side];
    _builtin_set_imask(15);
    if (side == 0) {
        value = link_side->color_bits.color_mode;
        if (value == 2) {
            if (window->composition.f.multi_level)
                window->composition.f.color = 1;
        } else {
            window->composition.f.color = 0;
        }
    }
    value = link_side->color_bits.dropout_color;
    job_set_dropout_color(value, side);
    if (win_params.side[0].composition.f.color != 0) {
        window->contrast = link_side->color_contrast;
        window->brightness_negated = link_side->color_brightness;
    } else {
        window->contrast = link_window_params_ptrs[side]->contrast;
        window->brightness_negated = 0;
    }
    _builtin_set_imask(saved_imask);
    value = (link_side->gamma_flags.b >> 4 & 131) | (link_side->gamma_flags.b & 128);
    if (link_side->gamma_flags.f.top)
        value = 128;
    window->lut_pattern.v &= 120;
    window->lut_pattern.v |= value;
}

/* Builds the reply frame for reply code link_reply (member c.code) (link_reply_table[code]) into link_tx_frame: word 0 is
   length << 8 | frame type, the data follows. */
void link_build_reply(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    link_reply.f.b7 = 0;
    _builtin_set_imask(saved_imask);
    link_reply_table[link_reply.c.code]();
    uart_tx_ptr = &link_tx_frame;
}

void link_build_reply_1(void)
{
    *(unsigned short *)&link_tx_frame = 0x252;
    uart_rx_count = 2;
}
void link_build_reply_none(void) {}


void link_build_reply_2(void)
{
    short *frame;

    frame = &link_tx_frame;
    if (settings_byte_0x33[0] & 0x80)
        *frame = 0x64e;
    else
        *frame = 0x44e;
    link_build_status_bytes();
}

void link_build_reply_3(void)
{
    short *frame;

    frame = &link_tx_frame;
    if (settings_byte_0x33[0] & 0x80)
        *frame = 0x646;
    else
        *frame = 0x446;
    link_build_status_bytes();
}

void link_build_status_bytes(void)
{
    unsigned char bits;

    if (win_params.paper_size.f.custom_size)
        bits = 0;
    else
        bits = (win_params.paper_size.v << 3) | 0x80;
    link_tx_byte_2.c = bits = bits + (win_params.resolution_mode.c.lo & 7);
    bits = 2;
    if (hw_option_flags.f.b1)
        bits |= 1;
    if (win_params.side[0].composition.f.halftone)
        bits |= 16;
    if (win_params.flags.f.adf != 0) {
        bits |= 4;
        if (job_separation_sheet_flags & 64)
            bits |= 64;
    }
    if (!power_state.f.saving) {
        if (!mdc_mailbox.sensor_status.f.adf_cover_open) {
            if (!mdc_mailbox.sensor_status.f.no_paper)
                bits |= 0x80;
        } else {
            bits |= 0x20;
        }
    } else if (hw_port_f->b1 == 0) {
        bits |= 0x80;
    }
    if (settings_test_flags[0] & 4) {
        bits |= 0x80;
        bits &= 0xdf;
    }
    if (win_params.paper_size.f.landscape)
        bits |= 8;
    link_tx_byte_2.a[1] = bits;
    bits = 0;
    link_tx_byte_4[0] = bits;
    bits = 0;
    link_tx_byte_5[0] = bits;
    if (settings_byte_0x33[0] & 0x80)
        uart_rx_count = 6;
    else
        uart_rx_count = 4;
}


int link_build_page_info(void)
{
    short error;
    unsigned short saved_scan_length;
    unsigned short saved_composition;
    unsigned short saved_binarize;

    *(unsigned short *)&link_tx_frame = 0xc4c;
    if (!(scsi_param_byte_2[0] & 64)) {
        saved_binarize = win_params.side[0].binarize.v;
        saved_composition = win_params.side[0].composition.v;
        link_page_info_flags.computing = 1;
        error = img_compute_side_params(0, 0);
        link_page_info_flags.computing = 0;
        win_params.side[0].binarize.v = saved_binarize;
        win_params.side[0].composition.v = saved_composition;
        if (error != 0) {
            err_set_illegal_request(error);
            return link_build_reply_4();
        }
        link_build_page_info_side(0);
    } else {
        saved_scan_length = img_side_params[1].scan_length_lines;
        saved_binarize = win_params.side[1].binarize.v;
        saved_composition = win_params.side[1].composition.v;
        link_page_info_flags.computing = 1;
        error = img_compute_side_params(1, 0);
        link_page_info_flags.computing = 0;
        win_params.side[1].binarize.v = saved_binarize;
        win_params.side[1].composition.v = saved_composition;
        if (error != 0) {
            img_side_params[1].scan_length_lines = saved_scan_length;
            err_set_illegal_request(error);
            return link_build_reply_4();
        }
        link_build_page_info_side(1);
        img_side_params[1].scan_length_lines = saved_scan_length;
    }
    uart_rx_count = 12;
}


void link_build_page_info_side(int side)
{
    short flags;
    union img_path_flags_word path;

    flags = 0;
    path.v = img_mode[side].path_flags.v;
    if (hw_option_flags.f.b1)
        flags |= 0x80;
    if (path.f.area_separation)
        flags |= 0x40;
    if (path.f.b9)
        flags |= 0x20;
    if (path.f.reverse_image)
        flags |= 0x10;
    if (path.f.emphasis || path.f.outline)
        flags |= 8;
    if (path.f.sdtc)
        flags |= 2;
    ((unsigned char *)link_tx_byte_2.a)[0] = flags;
    link_tx_byte_2.a[1] = 0;
    if (win_params.resolution_mode.f.dpi_given) {
        link_tx_byte_4[0] = (win_params.y_res_dpi.v >> 8) | 0x80;
        link_tx_byte_5[0] = win_params.y_res_dpi.b.lo;
        ((unsigned char *)link_tx_byte_6)[0] = win_params.x_res_dpi.v >> 8;
        ((unsigned char *)link_tx_byte_6)[1] = win_params.x_res_dpi.b.lo;
    } else {
        link_tx_byte_4[0] = win_params.y_zoom_percent.v >> 8;
        link_tx_byte_5[0] = win_params.y_zoom_percent.b.lo;
        if (win_params.resolution_mode.f.x_zoom_given) {
            ((unsigned char *)link_tx_byte_6)[0] = win_params.x_zoom_percent.v >> 8;
            ((unsigned char *)link_tx_byte_6)[1] = win_params.x_zoom_percent.b.lo;
        } else {
            ((unsigned char *)link_tx_byte_6)[0] = link_tx_byte_4[0];
            ((unsigned char *)link_tx_byte_6)[1] = link_tx_byte_5[0];
        }
    }
    ((unsigned char *)link_tx_byte_8)[0] = img_side_params[side].width_px >> 8;
    ((unsigned char *)link_tx_byte_8)[1] = (unsigned char)img_side_params[side].width_px;
    if (win_params.flags.f.adf && win_params.paper_size.f.auto_length) {
        if (job_auto_size_flags.b7) {
            ((unsigned char *)link_tx_byte_10)[0] = (unsigned short)line_page_timing[0].lines >> 8;
            ((unsigned char *)link_tx_byte_10)[1] = line_page_timing[0].lines;
        } else {
            ((unsigned char *)link_tx_byte_10)[0] = 0;
            ((unsigned char *)link_tx_byte_10)[1] = 0;
        }
    } else {
        ((unsigned char *)link_tx_byte_10)[0] = img_side_params[side].scan_length_lines >> 8;
        ((unsigned char *)link_tx_byte_10)[1] = (unsigned char)img_side_params[side].scan_length_lines;
    }
}

void link_build_reply_11(void)
{
    *(unsigned short *)&link_tx_frame = 0x74a;
    ((unsigned char *)link_tx_byte_2.a)[0] = 0;
    link_tx_byte_2.a[1] = (unsigned short)line_page_timing[0].lines >> 8;
    link_tx_byte_4[0] = line_page_timing[0].lines;
    link_tx_byte_5[0] = 0;
    ((unsigned char *)link_tx_byte_6)[0] = 0;
    if (win_params.flags.f.adf) {
        if (win_params.side[0].use_window_area == 0 || (win_params.side[0].use_window_area != 0 && !win_params.paper_size.f.long_page)) {
            link_tx_byte_5[0] = link_reply_11_value.v >> 8;
            ((unsigned char *)link_tx_byte_6)[0] = link_reply_11_value.c.lo;
        }
    }
    uart_rx_count = ((unsigned char *)&link_tx_frame)[0];
}

void link_build_capabilities(void)
{
    unsigned char value;

    *(unsigned short *)&link_tx_frame = 0x1e4a;
    value = 7;
    ((unsigned char *)link_tx_byte_2.a)[0] = value;
    link_tx_byte_2.a[1] = 47;
    link_tx_byte_4[0] = 0;
    link_tx_byte_5[0] = 0xf8;
    ((unsigned char *)link_tx_byte_6)[0] = 0xf0;
    ((unsigned char *)link_tx_byte_6)[1] = 2;
    if (!hw_option_flags.f.b1) {
        ((unsigned char *)link_tx_byte_8)[0] = 16;
        ((unsigned char *)link_tx_byte_8)[1] = 0;
    } else {
        ((unsigned char *)link_tx_byte_8)[0] = 0xdf;
        ((unsigned char *)link_tx_byte_8)[1] = 127;
    }
    ((unsigned char *)link_tx_byte_10)[0] = 1;
    if (!hw_option_flags.f.b1) {
        ((unsigned char *)link_tx_byte_10)[1] = 16;
        link_tx_byte_12[0] = 0;
    } else {
        ((unsigned char *)link_tx_byte_10)[1] = 0xdf;
        link_tx_byte_12[0] = 127;
    }
    link_tx_byte_13[0] = 1;
    if (!hw_option_flags.f.b0) {
        link_tx_byte_14[0] = 0;
    } else {
        link_tx_byte_14[0] = 1;
    }
    value = 99;
    if (job_black_backing) {
        value |= 12;
    }
    link_tx_byte_15[0] = value;
    link_tx_byte_16[0] = 0xff;
    link_tx_byte_17[0] = 0xff;
    link_tx_byte_18[0] = 0xe2;
    link_tx_byte_19[0] = 0;
    link_tx_byte_20[0] = 0;
    link_tx_byte_21[0] = 0;
    if (hw_hostif_flags.f.cga_test_scan) {
        ((unsigned char *)link_tx_byte_2.a)[0] = 6;
        link_tx_byte_2.a[1] = 15;
        link_tx_byte_5[0] = 0xf8;
        ((unsigned char *)link_tx_byte_6)[1] = 2;
        ((unsigned char *)link_tx_byte_8)[0] = 16;
        ((unsigned char *)link_tx_byte_8)[1] = 0;
        ((unsigned char *)link_tx_byte_10)[1] = 16;
        link_tx_byte_12[0] = 0;
        link_tx_byte_15[0] = 99;
        link_tx_byte_16[0] = 0;
        link_tx_byte_17[0] = 64;
        link_tx_byte_18[0] = 0;
    }
    for (value = 0; value < 8; value++) {
        link_tx_byte_22[value] = 0x20;
    }
    uart_rx_count = ((unsigned char *)&link_tx_frame)[0];
}

void link_build_identity(void)
{
    int saved_imask;
    unsigned char emulation;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    hw_cga_board_present.f.link_identified = 1;
    _builtin_set_imask(saved_imask);
    emulation = settings_emulation[0] & 0x1c;
    if (emulation == 0x1c || emulation == 0x18) {
        util_mem_copy(&link_tx_frame, link_identity_fi5750c, 30);
    } else {
        util_mem_copy(&link_tx_frame, link_identity_fi6770, 30);
    }
    util_mem_copy(link_tx_byte_16, scsi_revision, 4);
    if (power_state.f.saving) {
        link_tx_byte_20[0] = power_image_mdc_version_0[0].b[1];
        link_tx_byte_21[0] = power_image_mdc_version_1[0].w >> 8;
        link_tx_byte_22[0] = power_image_mdc_version_1[0].b[1];
        link_tx_byte_23[0] = power_image_mdc_version_2[0].w >> 8;
        if (hw_option_flags.f.b0) {
            link_tx_byte_24[0] = power_image_imprinter_version_0[0].w >> 8;
            link_tx_byte_25[0] = power_image_imprinter_version_0[0].b[1];
            link_tx_byte_26[0] = power_image_imprinter_version_1[0].w >> 8;
            link_tx_byte_27[0] = power_image_imprinter_version_1[0].b[1];
        } else {
            link_tx_byte_24[0] = 0xff;
            link_tx_byte_25[0] = 0xff;
            link_tx_byte_26[0] = 0xff;
            link_tx_byte_27[0] = 0xff;
        }
    } else {
        link_tx_byte_20[0] = mdc_version[0];
        link_tx_byte_21[0] = mdc_version_1[0];
        link_tx_byte_22[0] = mdc_version_2[0];
        link_tx_byte_23[0] = mdc_version_3[0];
        if (hw_option_flags.f.b0) {
            link_tx_byte_24[0] = mdc_imprinter_version[0];
            link_tx_byte_25[0] = mdc_imprinter_version_1[0];
            link_tx_byte_26[0] = mdc_imprinter_version_2[0];
            link_tx_byte_27[0] = mdc_imprinter_version_3[0];
        } else {
            link_tx_byte_24[0] = 0xff;
            link_tx_byte_25[0] = 0xff;
            link_tx_byte_26[0] = 0xff;
            link_tx_byte_27[0] = 0xff;
        }
    }
    uart_rx_count = ((unsigned char *)&link_tx_frame)[0];
}
