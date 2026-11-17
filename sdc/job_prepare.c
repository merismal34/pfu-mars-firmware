#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


void job_prepare_side(int start)
{
    int side;
    int last_side;
    side = 0;
    if (win_sides.f.selection != 0) {
        last_side = 1;
    } else {
        last_side = 0;
    }
    _builtin_set_imask(15);
    for (; side <= last_side; side++) {
        if (start != 0) {
            p_pisces_capture[side]->ctrl.enable = 0;
            p_pisces_pass_source[side]->ctrl.enable = 0;
            p_pisces_pass_dest[side]->ctrl.enable = 0;
            if (main_page_loop_phase) {
                p_pisces_mode[side]->reg_014.f.f8 = 0;
                p_pisces_mode[side]->format.f.b4 = 1;
                p_pisces_mode[side]->format.f.b2 = 1;
                p_pisces_mode[side]->format.f.f0 = 0;
                if (win_params.side[0].composition.f.color) {
                } else {
                    if (job_long_page_flags & 0x80) {
                        if (!win_params.side[0].composition.f.color && job_dropout[1].w != 3) {
                            p_pisces_mode[side]->format.f.b4 = 0;
                            p_pisces_mode[side]->format.f.b2 = 0;
                            switch (job_dropout[1].w) {
                            case 0:
                                p_pisces_mode[side]->format.f.f0 = 1;
                                break;
                            case 2:
                                p_pisces_mode[side]->format.f.f0 = 3;
                                break;
                            default:
                                p_pisces_mode[side]->format.f.f0 = 2;
                                break;
                            }
                        }
                    }
                }
                if (testmode_number.v == 15) {
                    if (win_params.side[0].composition.f.color) {
                        p_pisces_mode[side]->format.v = 0xb5f4;
                        p_pisces_mode[side]->reg_014.v = 0x2100;
                    } else {
                        p_pisces_mode[side]->reg_014.v = 0x2100;
                    }
                }
            }
            if (hw_hostif_flags.f.own_hostif) {
                unsigned char capture_buf;
                capture_buf = page_side_state[side].capture_buf;
                page_side_state[side].buf[capture_buf].flags = 0x80;
                job_prepare_side_step_1(side);
                page_side_state[side].events |= 0x80;
                p_pisces_capture[side]->base_64k.f.v = page_side_state[side].buf[page_side_state[side].capture_buf].base_64k.s.lo;
                p_pisces_mode[side]->run.f.capture_on = 1;
            }
            line_side_state[side].state_flags.v = 0;
        } else {
            line_stop_capture(0);
            line_stop_capture(1);
            p_pisces_mode[side]->run.f.capture_on = 0;
            if (main_page_loop_phase) {
                p_pisces_mode[side]->run.f.b4 = 0;
                p_pisces_mode[side]->run.f.b8 = 0;
            }
            if (testmode_number.v == 15) {
                p_pisces_mode[side]->run.f.b14 = 0;
            }
            p_pisces_ctrl[side]->event_enable.f.b5 = 0;
            p_pisces_ctrl[side]->event_enable.f.b2 = 0;
            p_pisces_ctrl[side]->event_enable.f.b1 = 0;
            p_pisces_ctrl[side]->event_enable.f.b0 = 0;
        }
    }
    _builtin_set_imask(0);
}


void job_prepare_side_step_1(int side)
{
    unsigned short *left_edges;
    unsigned short *right_edges;
    char *base;
    int unused_frame;
    unsigned char capture_buf;
    unsigned char unused2;
    unsigned char record;
    capture_buf = page_side_state[side].capture_buf;
    if (job_auto_size_flags.b7) {
        edge_side_records[side].e[capture_buf].state = EDGE_STATE_START;
        base = flash_download_buf;
        record = page_side_state[side].captured_count % 3;
        if (side == 0) {
            if (record == 0) { /* edge arrays, 0x578 shorts each: record 0 in RAM, records 1/2 in flash_download_buf */
                left_edges = edge_left_front;
                right_edges = edge_right_front;
            } else if (record == 1) {
                left_edges = (unsigned short *)base;
                right_edges = (unsigned short *)(base + 0xaf0);
            } else {
                left_edges = (unsigned short *)(base + 0x15e0);
                right_edges = (unsigned short *)(base + 0x20d0);
            }
        } else if (record == 0) {
            left_edges = edge_left_back;
            right_edges = edge_right_back;
        } else if (record == 1) {
            left_edges = (unsigned short *)(base + 0x2bc0);
            right_edges = (unsigned short *)(base + 0x36b0);
        } else {
            left_edges = (unsigned short *)(base + 0x41a0);
            right_edges = (unsigned short *)(base + 0x4c90);
        }
        edge_side_records[side].e[capture_buf].left_edges = left_edges;
        edge_side_records[side].e[capture_buf].right_edges = right_edges;
        if (!win_params.flags.f.adf && (settings_emulation[0] & 0x1c) != 0x18) {
            edge_side_records[side].e[capture_buf].state = EDGE_STATE_FALLBACK;
        }
    } else {
        edge_side_records[side].e[capture_buf].state = EDGE_STATE_FALLBACK;
    }
    edge_side_records[side].e[capture_buf].flags = 0;
    *(unsigned short *)&edge_side_records[side].e[capture_buf].top_lines = 0xffff;
    *(unsigned short *)&edge_side_records[side].e[capture_buf].event_20_lines = 0xffff;
}

struct vdcc_bank_mode_bits { unsigned char p:6, level_high:2; };
struct byte_field { unsigned char v:8; };
union vdcc_bank_byte { unsigned char v; struct byte_field level_low; struct vdcc_bank_mode_bits g; };

struct vdcc_field_x_y { unsigned char p:2, x:3, y:3; };
struct vdcc_regs_job_prepare {
    char p[8];
    struct vdcc_field_x_y emphasis_ctrl;
    struct vdcc_field_x_y reg_009;
    char reg_00a;
    struct vdcc_field_x_y output_ctrl;
};

/* Per side, the VDCC3 test pattern (settings_test_flags bit 1, pattern settings_test_pattern bits 1-2) and the +8..+11 fields
   from the test settings bytes. */
void job_apply_vdcc_debug_settings(void)
{
    int side;
    unsigned char pattern;
    for (side = 0; side <= 1; side++) {
        if (settings_test_flags[0] & 2) {
            pattern = settings_test_pattern[0] & 6;
            if (pattern == 2 || pattern == 4) {
                if (pattern == 2) {
                    ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[1].v = 0x80;
                } else {
                    ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[1].v = 0x95;
                }
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[2].g.level_high = 2;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[3].level_low.v = 0xa8;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[4].g.level_high = 1;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[5].level_low.v = 0x54;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[6].g.level_high = 2;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[7].level_low.v = 0xa8;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[8].g.level_high = 1;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[9].level_low.v = 0x54;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[10].g.level_high = 2;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[11].level_low.v = 0xa8;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[12].g.level_high = 1;
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[13].level_low.v = 0x54;
            } else if (pattern == 6) {
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[1].v = 0;
                job_load_vdcc_test_table(side);
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[1].v = 0xbf;
            } else {
                ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[1].v = 0xaa;
            }
        } else {
            ((union vdcc_bank_byte *)p_vdcc_front_bank[side])[1].v = 0;
        }
        if (settings_test_flags[0] & 1) {
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->reg_009.x = 0;
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->reg_009.y = 0;
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->emphasis_ctrl.x = 0;
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->emphasis_ctrl.y = 0;
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->output_ctrl.x = 0;
        } else {
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->reg_009.x = 7;
        }
        if (settings_test_flags[0] & 8) {
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->output_ctrl.y = 0;
        } else {
            ((struct vdcc_regs_job_prepare *)p_vdcc_front[side])->output_ctrl.y = 7;
        }
    }
}

void job_load_vdcc_test_table(int side)
{
    unsigned short *p;
    unsigned short v17;
    unsigned short v16;
    unsigned short v15;
    unsigned short v14;
    unsigned short v13;
    unsigned short v12;
    unsigned short v11;
    unsigned short v10;
    unsigned short v9;
    unsigned short v8;
    unsigned short v7;
    unsigned short v6;
    unsigned short v5;
    unsigned short v4;
    unsigned short v3;
    unsigned short v2;
    unsigned short i;
    unsigned short v0;
    v0 = 0x3ff;
    v2 = 0x1c0;
    v3 = 0x1c0;
    v4 = 0x180;
    v5 = 0xc0;
    v6 = 0xc0;
    v7 = 0x180;
    v8 = 0x1c0;
    v9 = 0x1c0;
    v10 = 0x1c0;
    v11 = 0x100;
    v12 = 0x1c0;
    v13 = 0x1c0;
    v14 = 0x1c0;
    v15 = 0x120;
    v16 = 0x120;
    v17 = 0x1c0;
    p_vdcc_front_bank[side]->bank = 0;
    _builtin_set_imask(0);
    testmode_wait_lines(2);
    if (side == 0) {
        p = p_vdcc_front_test_table;
    } else {
        p = p_vdcc_back_test_table;
    }
    for (i = 0; i < 0x50; i++) {
        *p++ = 0x80;
    }
    for (i = 0x50; i < 0xc0; i++) {
        *p++ = 0x280;
    }
    for (i = 0xc0; i < 0x100; i++) {
        *p++ = 0xc0;
    }
}


void job_wait_mdc_lines(void)
{
    unsigned short lines;
    if (!job_separation_motion_wait) {
        return;
    } else
    while (mdc_mailbox.motion_flags & MDC_MOTION_FRONT) {
        if (tick_page_wait_ticks.v >= 0xdac) {
            _builtin_set_imask(15);
            job_separation_motion_wait = 0;
            _builtin_set_imask(0);
            return;
        }
    }
    _builtin_set_imask(15);
    job_separation_motion_wait = 0;
    job_scan_complete_wait = 0;
    _builtin_set_imask(0);
    lines = 0x3b1;
    lines = rt_divu32(lines * img_side_params[0].y_res_dpi, 600);
    _builtin_set_imask(15);
    line_side_state[0].line_count = 0;
    _builtin_set_imask(0);
    while (line_side_state[0].line_count < lines) {
    }
}


void job_wait_line_count(void)
{
    int side;
    if (settings_test_flags[0] & 4) {
        return;
    }
    if (!win_params.flags.f.adf) {
        return;
    }
    if (win_sides.f.selection == 0) {
        side = 0;
    } else {
        side = 1;
    }
    while (line_motion_stop_lines[side] < line_side_state[side].stop_target_lines) {
        if (tick_page_wait_ticks.v >= 0xdac) {
            calib_levels_deferred = 1;
            break;
        }
    }
}

struct vdcc_monitor_reg_00 { unsigned char hi:4, lo:4; };
struct vdcc_monitor_reg_01 { unsigned char p:2, mode:2, lo:4; };
struct vdcc_monitor_view {
    struct vdcc_monitor_reg_00 reg_060;
    struct vdcc_monitor_reg_01 reg_061;
    unsigned short level_left, level_middle, level_right, position_left, position_middle, position_right;
};

char job_calibrate_levels_step_15(void)
{
    int side;
    unsigned int sample;
    unsigned int sums[2][3];
    int unused_frame;
    char code;
    code = 0;
    if ((settings_byte_0x1d[0] & 0x10) || (settings_test_flags[0] & 0x10) || (settings_test_flags[0] & 2)) {
        return code;
    }
    for (side = 0; side <= 1; side++) {
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->reg_061.mode = 2;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->reg_061.lo = 8;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->reg_060.lo = 8;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->reg_060.hi = 8;
        if (img_side_params[0].ccd_300dpi_mode) {
            ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->position_left = 0x2ee;
            ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->position_middle = 0x6a0;
            ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->position_right = 0xae8;
        } else {
            ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->position_left = 0x5dc;
            ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->position_middle = 0xd40;
            ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->position_right = 0x15d0;
        }
    }
    for (side = 0; side <= 1; side++) {
        sums[side][0] = 0;
        sums[side][1] = 0;
        sums[side][2] = 0;
    }
    for (sample = 0; sample < 4; sample++) {
        for (side = 0; side <= 1; side++) {
            sums[side][0] += ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->level_left;
            sums[side][1] += ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->level_middle;
            sums[side][2] += ((struct vdcc_monitor_view *)p_vdcc_front_monitor[side])->level_right;
        }
        rt_delay_24k();
    }
    for (side = 0; side <= 1; side++) {
        sums[side][0] >>= 2;
        sums[side][1] >>= 2;
        sums[side][2] >>= 2;
    }
    if (sums[0][0] < 0x100 || sums[0][1] < 0x100 || sums[0][2] < 0x100) {
        if (!win_params.flags.f.adf) {
            code = 0x7e;
        } else if (win_sides.f.selection != 1) {
            code = 0x74;
        }
    }
    if ((sums[1][0] < 0x100 || sums[1][1] < 0x100 || sums[1][2] < 0x100) && win_sides.f.selection) {
        code = 0x75;
    }
    return code;
}

struct pisces_kernel_ctrl_reg_06 {
    char p[6];
    struct { unsigned short p:8, v:8; } reg_166;
};

void job_check_mdc_handshake_step_8(void)
{
    unsigned short i;
    unsigned short level;
    unsigned short margin;
    unsigned short lo;
    unsigned short hi;
    unsigned char hist[256];
    unsigned char max;
    char peak;
    unsigned char *bins;
    unsigned int sample;
    bins = hist;
    util_mem_clear(bins, 0x100);
    if (line_long_page_level_index < 0x578) {
        hi = line_long_page_level_index;
    } else {
        hi = 0x578;
    }
    for (i = 0; i < line_long_page_level_index; i++) {
        sample = edge_left_back[i];
        bins[sample]++;
    }
    level = (unsigned short)((volatile struct pisces_kernel_ctrl_reg_06 *)p_pisces_kernel_ctrl[0])->reg_166.v;
    margin = rt_divu16(level * 0x5e + 0x22b, 1000);
    level += margin;
    margin = rt_divu16(level * 0x50 + 0x22b, 1000);
    if (level > margin) {
        lo = level - margin;
    } else {
        lo = level;
    }
    hi = level + margin;
    if (hi > 0xff) {
        hi = 0xff;
    }
    max = 0;
    peak = 0;
    for (i = lo; i < hi; i++) {
        if (bins[i] > max) {
            max = bins[i];
            peak = i;
        }
    }
    img_long_page_level = peak;
}


int job_check_mdc_handshake(void)
{
    int j;
    int i;
    int k;
    int no_motion;
    struct page_record *scanning_page;
    no_motion = 0;
    scanning_page = &page_records[page_scanning.v];
    job_long_page_flags |= 4;
    if (job_aborted != 0) {
        return job_abort();
    } else if (main_page_loop_phase == 2) {
        if (win_sides.f.selection == 2) {
            i = 1;
            j = 0;
        } else if (win_sides.f.selection == 1) {
            i = 1;
            j = 1;
        } else {
            i = 0;
            j = 0;
        }
    } else {
        if (win_sides.f.selection == 1) {
            if (!(mdc_mailbox.motion_flags & MDC_MOTION_BACK)) {
                no_motion = 1;
            }
        } else {
            if (!(mdc_mailbox.motion_flags & MDC_MOTION_FRONT)) {
                no_motion = 1;
            }
            if (win_sides.f.selection == 2 && !(mdc_mailbox.motion_flags & MDC_MOTION_BACK)) {
                no_motion = 1;
            }
        }
        if (no_motion) {
            for (i = 0; i < 20; i++) {
                if (!hw_port_f->b1) {
                    no_motion = 0;
                    break;
                }
                rt_delay_60k();
                rt_delay_60k();
            }
        }
        if (no_motion) {
            if (mdc_capture_pause_sent == 1) {
                mdc_request_capture(1);
                for (i = 0; i <= 50; i++) {
                    if (!mdc_mailbox.request) {
                        break;
                    }
                    rt_delay_180k();
                    rt_delay_60k();
                }
            }
            job_feed_starting = 1;
            if (mdc_run_feed() == 0) {
                err_check_pending();
            } else {
                err_raise(ERR_NO_PAPER);
            }
            job_feed_starting = 0;
            return job_abort();
        }
        if ((job_long_page_flags & 0x80) && win_white_level_follow != 0xc0) {
            job_check_mdc_handshake_step_8();
        }
        _builtin_set_imask(15);
        page_records[page_scanning.v].flags.f.sheet_fed = 1;
        scanning_page->sides[0].size_final = 0;
        _builtin_set_imask(0);
        for (;;) {
            _builtin_set_imask(15);
            if ((main_page_loop_phase == 1 && scanning_page->sides[0].flags.f.requested && win_sides.f.selection != 1)
                || (main_page_loop_phase == 1 && scanning_page->sides[1].flags.f.requested && win_sides.f.selection == 1)
                || (main_page_loop_phase != 1 && scanning_page->flags.f.scan_requested)) {
                break;
            }
            _builtin_set_imask(0);
            if (scanning_page->flags.f.scan_end != 0 || job_aborted) {
                return job_abort();
            }
        }
        _builtin_set_imask(0);
        job_prepare_side(0);
        if (job_setup_sensor() == 0) {
            return job_abort();
        }
        if (main_page_loop_phase == 1) {
            return 1;
        }
    }
    job_prepare_side(1);
    while (tick_line_event_ticks.v < 0x96) {
        if (scanning_page->flags.f.scan_end != 0 || job_aborted) {
            return job_abort();
        }
    }
    if (job_start_page() == 0) {
        return job_abort();
    }
    if (job_wait_side_complete(0) == 0) {
        return job_abort();
    }
    if (job_end() == 0) {
        return job_abort();
    }
    return 1;
}
