#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


/* Uninitialised data (section B). */
int page_poll_imask;
unsigned short page_jpeg_count_retries[2];
unsigned short page_poll_wait_count;

void page_poll_progress(void)
{
    struct page_record *current_page;
    int side;
    int last_side;

    current_page = &page_records[page_current.v];
    page_poll_imask = _builtin_get_imask();
    page_poll_count++;
    if (hw_hostif_flags.f.own_hostif == 0) {
        return;
    }
    page_update_image_counts();
    if (page_poll_count % 8 != 0) {
        return;
    }
    if ((maint_mode_word.v & 0x5000) != 0) {
        if ((maint_mode_word.c.mode < 2 || maint_mode_word.c.mode > 4) && !hw_hostif_flags.f.own_hostif) {
            return;
        }
    }
    if (testmode_number.v != 0) {
        return;
    }
    if (power_state.f.saving != 0) {
        return;
        _builtin_set_imask(page_poll_imask);
    }
    _builtin_set_imask(15);
    if (current_page->flags.f.load_pending && current_page->flags.f.sheet_fed) {
        current_page->flags.f.load_pending = 0;
        scsi_resume_flags.f.resume_pending = 1;
        _builtin_set_imask(page_poll_imask);
        isr_request_command_layer(0);
    }
    _builtin_set_imask(page_poll_imask);
    _builtin_set_imask(15);
    if ((current_page->flags.v & PAGE_FLAGS_REPLY_PENDING) != 0) {
        if ((scsi_unusual_paper_state & 0x80) && current_page->flags.f.load_pending) {
        } else {
            if (current_page->flags.f.scan_end) {
                if (current_page->error_flags.v == 0 && job_aborted != 0) {
                    if (++page_poll_wait_count < 0x9c4) {
                        _builtin_set_imask(page_poll_imask);
                        return;
                    }
                }
                page_poll_wait_count = 0;
                scsi_resume_flags.f.page_ended = 1;
                current_page->flags.v &= ~PAGE_FLAGS_SCAN;
                current_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
                util_mem_clear(&current_page->sides[0].flags, 48);
                util_mem_clear(&current_page->sides[1].flags, 48);
                _builtin_set_imask(page_poll_imask);
                isr_request_command_layer(0);
            }
        }
    }
    _builtin_set_imask(page_poll_imask);
    if (page_size_ready != 0) {
        _builtin_set_imask(15);
        page_size_ready = 0;
        page_size_reply_pending = 1;
        _builtin_set_imask(page_poll_imask);
        isr_request_command_layer(0);
    }
    if (!current_page->sides[0].flags.f.requested && !current_page->sides[1].flags.f.requested) {
        return;
    }
    if (win_sides.f.selection == 2) {
        last_side = 1;
        side = 0;
    } else if (win_sides.f.selection == 1) {
        last_side = 1;
        side = 1;
    } else {
        last_side = 0;
        side = 0;
    }
    while (side <= last_side) {
        if (current_page->sides[side].flags.f.requested && !current_page->sides[side].flags.f.data_ready && current_page->sides[side].image_bytes != 0) {
            jpeg_setup_next_page(side);
            current_page->sides[side].flags.f.data_ready = 1;
            return;
        }
        if (page_buf_limit_bytes != 0) {
            jpeg_check_buffer_limit(side);
        }
        side++;
    }
}

void page_update_image_counts(void)
{
    struct page_record *current_page;
    unsigned long bytes;
    int u40, u36, u32;
    int side;
    int last_side;
    int u20, u16;
    unsigned short count_high, count_low, count_high_2, count_low_2;
    unsigned long bytes_2;
    int ok;

    current_page = &page_records[page_current.v];
    if (!current_page->sides[0].flags.f.requested && !current_page->sides[1].flags.f.requested) {
        return;
    }
    _builtin_set_imask(15);
    if (current_page->flags.f.read_pending) {
        if (win_params.side[scsi_read_side.v].compression.f.type & 0x80) {
            if (scsi_xfer_len.v <= current_page->sides[scsi_read_side.v].jpeg_bytes - current_page->sides[scsi_read_side.v].jpeg_bytes_sent) {
                current_page->flags.f.read_pending = 0;
                scsi_resume_flags.f.resume_pending = 1;
                _builtin_set_imask(page_poll_imask);
                isr_request_command_layer(0);
            } else {
                if (current_page->sides[scsi_read_side.v].jpeg_done != 0) {
                    current_page->flags.f.read_pending = 0;
                    scsi_resume_flags.f.resume_pending = 1;
                    _builtin_set_imask(page_poll_imask);
                    isr_request_command_layer(0);
                }
            }
        } else {
            if (scsi_xfer_len.v <= current_page->sides[scsi_read_side.v].image_bytes - current_page->sides[scsi_read_side.v].image_bytes_sent) {
                current_page->flags.f.read_pending = 0;
                scsi_resume_flags.f.resume_pending = 1;
                _builtin_set_imask(page_poll_imask);
                isr_request_command_layer(0);
            } else {
                if (current_page->sides[scsi_read_side.v].flags.f.scanned) {
                    current_page->flags.f.read_pending = 0;
                    scsi_resume_flags.f.resume_pending = 1;
                    _builtin_set_imask(page_poll_imask);
                    isr_request_command_layer(0);
                }
            }
        }
        if (win_sides.f.selection == 2 && (jpeg_scan_flags.v & JPEG_SCAN_PAUSED) && hw_hostif_flags.f.usb) {
            current_page->flags.f.read_pending = 0;
            scsi_resume_flags.f.resume_pending = 1;
            _builtin_set_imask(page_poll_imask);
            isr_request_command_layer(0);
        }
    }
    _builtin_set_imask(page_poll_imask);
    if (win_sides.f.selection == 2) {
        last_side = 1;
        side = 0;
    } else if (win_sides.f.selection == 1) {
        last_side = 1;
        side = 1;
    } else {
        last_side = 0;
        side = 0;
    }
    for (; side <= last_side; side++) {
        _builtin_set_imask(15);
        if (current_page->sides[side].flags.f.requested && current_page->sides[side].flags.f.data_ready) {
        } else {
            continue;
        }
        if (current_page->sides[side].size_final != 0 && current_page->sides[side].output_lines_set == 0) {
            p_pisces_output[side]->ctrl.enable = 1;
            p_pisces_output[side]->lines = p_pisces_output[side]->reg_0d4 = current_page->sides[side].length_lines;
            p_pisces_output[side]->ctrl.enable = 0;
            current_page->sides[side].output_lines_set = 1;
        }
        if (win_params.side[side].compression.f.type & 0x80) {
            if (current_page->sides[side].jpeg_done == 0) {
                if (p_pisces_ctrl[side]->event_status.f.b6) {
                    p_pisces_ctrl[side]->ctrl.f.jpeg_access = 1;
                    if (p_jpeg[side]->reg_1f == 64) {
                        current_page->sides[side].l40 = 1;
                        if (!p_pisces_ctrl[side]->event_status.f.b7) {
                            _builtin_set_imask(15);
                            p_pisces_ctrl[side]->ctrl.f.jpeg_access = 0;
                            goto count_bytes;
                        }
                        if (!current_page->sides[side].flags.f.scanned) {
                            _builtin_set_imask(15);
                            goto count_bytes;
                        }
                        p_jpeg[side]->reg_1f = 0;
                        p_jpeg[side]->reg_03.b2 = 1;
                        _builtin_set_imask(page_poll_imask);
                        while (1) {
                            if (p_pisces_ctrl[side]->event_status.f.b7) {
                                _builtin_set_imask(15);
                                p_pisces_ctrl[side]->event_enable.f.b7 = 0;
                                p_pisces_ctrl[side]->event_enable.f.b3 = 0;
                                p_pisces_ctrl[side]->event_enable.f.b6 = 0;
                                p_pisces_ctrl[side]->event_enable.f.b7 = 1;
                                p_pisces_ctrl[side]->event_enable.f.b3 = 1;
                                p_pisces_ctrl[side]->event_enable.f.b6 = 1;
                                break;
                            }
                        }
                        _builtin_set_imask(15);
                    } else {
                        _builtin_set_imask(page_poll_imask);
                        while (1) {
                            if (!p_pisces_ctrl[side]->event_status.f.b7 && !p_pisces_ctrl[side]->event_status.f.b3 && !p_pisces_ctrl[side]->event_status.f.b6) {
                                break;
                            } else {
                                _builtin_set_imask(15);
                                p_pisces_ctrl[side]->event_enable.f.b7 = 0;
                                p_pisces_ctrl[side]->event_enable.f.b3 = 0;
                                p_pisces_ctrl[side]->event_enable.f.b6 = 0;
                                p_pisces_ctrl[side]->event_enable.f.b7 = 1;
                                p_pisces_ctrl[side]->event_enable.f.b3 = 1;
                                p_pisces_ctrl[side]->event_enable.f.b6 = 1;
                                _builtin_set_imask(page_poll_imask);
                            }
                        }
                        _builtin_set_imask(15);
                        p_pisces_ctrl[side]->ctrl.f.jpeg_access = 0;
                        _builtin_set_imask(page_poll_imask);
                        return;
                    }
                    _builtin_set_imask(15);
                    p_pisces_ctrl[side]->ctrl.f.jpeg_access = 0;
                    current_page->sides[side].jpeg_done = 1;
                }
            count_bytes:
                _builtin_set_imask(15);
                bytes = p_pisces_output[side]->width_px;
                bytes = bytes * p_pisces_output[side]->lines_done;
                if (win_params.side[0].composition.f.color) {
                    bytes = bytes * 3;
                }
                _builtin_set_imask(15);
                if (bytes > current_page->sides[side].image_bytes) {
                } else {
                    current_page->sides[side].image_bytes_sent = bytes;
                }
                count_high = p_pisces_hostif[side]->count_high;
                count_low = p_pisces_hostif[side]->count_low;
                bytes = ((unsigned long)count_high << 16) | count_low;
                if (current_page->sides[side].jpeg_done != 0 || page_jpeg_count_retries[side] > 100) {
                    ok = 1;
                } else {
                    count_high_2 = p_pisces_hostif[side]->count_high;
                    count_low_2 = p_pisces_hostif[side]->count_low;
                    bytes_2 = ((unsigned long)count_high_2 << 16) | count_low_2;
                    if (bytes == bytes_2) {
                        ok = 1;
                    } else {
                        count_high = p_pisces_hostif[side]->count_high;
                        count_low = p_pisces_hostif[side]->count_low;
                        bytes = ((unsigned long)count_high << 16) | count_low;
                        if (bytes == bytes_2) {
                            ok = 1;
                        } else {
                            ok = 0;
                            page_jpeg_count_retries[side]++;
                        }
                    }
                }
                if (ok) {
                    if (!count_low && bytes < current_page->sides[side].jpeg_bytes) {
                        count_high = p_pisces_hostif[side]->count_high;
                        count_low = p_pisces_hostif[side]->count_low;
                        bytes = ((unsigned long)count_high << 16) | count_low;
                        if (current_page->sides[side].jpeg_done != 0 || page_jpeg_count_retries[side] > 100) {
                            ok = 1;
                        } else {
                            count_high_2 = p_pisces_hostif[side]->count_high;
                            count_low_2 = p_pisces_hostif[side]->count_low;
                            bytes_2 = ((unsigned long)count_high_2 << 16) | count_low_2;
                            if (bytes == bytes_2) {
                                ok = 1;
                            } else {
                                ok = 0;
                                page_jpeg_count_retries[side]++;
                            }
                        }
                    }
                    if (ok) {
                        page_jpeg_count_retries[side] = 0;
                        current_page->sides[side].jpeg_bytes = bytes;
                        if (current_page->sides[side].jpeg_bytes < current_page->sides[side].jpeg_bytes_sent) {
                            count_high = p_pisces_hostif[side]->count_high;
                            count_low = p_pisces_hostif[side]->count_low;
                            current_page->sides[side].jpeg_bytes = bytes;
                        }
                    }
                }
            }
        }
    }
    _builtin_set_imask(page_poll_imask);
}
