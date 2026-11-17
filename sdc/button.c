#define panel_set_display_mode panel_set_display_mode_hdr
#include "fw.h"
#undef panel_set_display_mode
void panel_set_display_mode(unsigned char, unsigned char);
extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);


/* Every 8 ms. Raw bits (1 = pressed): b0 Scan/Stop, b1 Send to, b3 Function, b7 Power. Each is debounced
   into button_state (bits 7, 6, 4, 3) after 7 polls; the latches GET HARDWARE STATUS reports are set after
   8 polls held and cleared 375 polls (3 s) after release; Power counts after 250 polls (2 s). */
void button_poll(void)
{
    union { unsigned char c; struct Bits b; } button_bits;
    int saved_imask;
    struct page_record *scanning_page;
    struct page_record *current_page;

    saved_imask = _builtin_get_imask();
    scanning_page = &(*(struct page_record *)&page_records) + page_scanning.v;
    current_page = &(*(struct page_record *)&page_records) + page_current.v;
    if (power_state.f.unused) {
        panel_current.button_scan_latch = 0;
        panel_current.button_send_to_latch = 0;
        panel_current.button_function_request = 0;
        panel_current.button_function_back_request = 0;
        return;
    }
    _builtin_set_imask(15);
    if (iic_busy.b7) {
        _builtin_set_imask(saved_imask);
        return;
    }
    _builtin_set_imask(saved_imask);
    button_bits.c = iic_panel_read_buttons();
    /* paused at a page end: Function and Send to ignored */
    if (job_button_pause_flags & 1) {
        _builtin_set_imask(15);
        button_bits.b.b3 = 0;
        button_bits.b.b1 = 0;
        _builtin_set_imask(saved_imask);
    }
    if (hw_power_switch_enabled() != 0) {
        if (button_bits.b.b7 == 0) {  /* Power up */
            panel_current.button_power_press_latch_polls = 0;
            if (panel_current.page_activity == 0 && (err_display_flags.v & 0xc0) == 0) {
                if (++panel_current.button_power_press_unlatch_polls >= BUTTON_UNLATCH_POLLS) {
                    panel_current.button_power_press_latch = 0;
                    panel_current.button_power_press_unlatch_polls = 0;
                }
            }
            if (panel_current.button_power_release_polls > BUTTON_DEBOUNCE_POLLS) {
                _builtin_set_imask(15);
                button_power_latch.v = 1;
                button_state.b.b3 = 0;
                _builtin_set_imask(saved_imask);
                panel_current.button_power_press_polls = 0;
            } else {
                panel_current.button_power_release_polls++;
            }
        } else {
            panel_current.button_power_press_unlatch_polls = 0;
            if (panel_current.page_activity == 0 && (err_display_flags.v & 0xc0) == 0) {
                if (++panel_current.button_power_press_latch_polls >= BUTTON_LATCH_POLLS) {
                    panel_current.button_power_press_latch = 1;
                    panel_current.button_power_press_latch_polls = 0;
                }
            }
            if (testmode_number.v == 37 || testmode_number.v == 3) {  /* TESTMODE panel tests: no 2 s hold */
                if (panel_current.button_power_press_polls > BUTTON_DEBOUNCE_POLLS) {
                    _builtin_set_imask(15);
                    button_state.b.b3 = 1;
                    _builtin_set_imask(saved_imask);
                    panel_current.button_power_release_polls = 0;
                } else {
                    panel_current.button_power_press_polls++;
                }
            } else {
                if (panel_current.button_power_press_polls > BUTTON_POWER_OFF_POLLS) {
                    _builtin_set_imask(15);
                    button_state.b.b3 = 1;
                    _builtin_set_imask(saved_imask);
                    panel_current.button_power_release_polls = 0;
                } else {
                    panel_current.button_power_press_polls++;
                }
            }
        }
    } else {
        _builtin_set_imask(15);
        button_state.b.b3 = 0;
        _builtin_set_imask(saved_imask);
    }
    if (button_bits.b.b0 == 0) {  /* Scan/Stop up */
        panel_current.button_scan_latch_polls = 0;
        if (panel_current.page_activity == 0 && (err_display_flags.v & 0x40) == 0) {
            if (++panel_current.button_scan_unlatch_polls >= BUTTON_UNLATCH_POLLS) {
                panel_current.button_scan_latch = 0;
                panel_current.button_scan_unlatch_polls = 0;
            }
        }
        if (panel_current.button_scan_release_polls > BUTTON_DEBOUNCE_POLLS) {
            _builtin_set_imask(15);
            button_state.b.b7 = 0;
            _builtin_set_imask(saved_imask);
            panel_current.button_scan_press_polls = 0;
        } else {
            panel_current.button_scan_release_polls++;
        }
    } else {
        panel_current.button_scan_unlatch_polls = 0;
        if (panel_current.page_activity == 0 && (err_display_flags.v & 0x40) == 0) {
            if (++panel_current.button_scan_latch_polls >= BUTTON_LATCH_POLLS) {
                panel_current.button_scan_latch = 1;
                panel_current.button_scan_latch_polls = 0;
            }
        }
        if (panel_current.button_scan_press_polls > BUTTON_DEBOUNCE_POLLS) {
            _builtin_set_imask(15);
            button_state.b.b7 = 1;
            _builtin_set_imask(saved_imask);
            panel_current.button_scan_release_polls = 0;
        } else {
            panel_current.button_scan_press_polls++;
        }
    }
    if (button_bits.b.b1 == 0) {  /* Send to up */
        panel_current.button_send_to_latch_polls = 0;
        if (panel_current.page_activity == 0 && (err_display_flags.v & 0x40) == 0) {
            if (++panel_current.button_send_to_unlatch_polls >= BUTTON_UNLATCH_POLLS) {
                panel_current.button_send_to_latch = 0;
                panel_current.button_send_to_unlatch_polls = 0;
            }
        }
        if (panel_current.button_send_to_release_polls > BUTTON_DEBOUNCE_POLLS) {
            _builtin_set_imask(15);
            button_state.b.b6 = 0;
            _builtin_set_imask(saved_imask);
            panel_current.button_send_to_press_polls = 0;
        } else {
            panel_current.button_send_to_release_polls++;
        }
    } else {
        panel_current.button_send_to_unlatch_polls = 0;
        if (panel_current.page_activity == 0 && (err_display_flags.v & 0x40) == 0) {
            if (++panel_current.button_send_to_latch_polls >= BUTTON_LATCH_POLLS) {
                panel_current.button_send_to_latch = 1;
                panel_current.button_send_to_latch_polls = 0;
            }
        }
        if (panel_current.button_send_to_press_polls > BUTTON_DEBOUNCE_POLLS) {
            _builtin_set_imask(15);
            button_state.b.b6 = 1;
            _builtin_set_imask(saved_imask);
            panel_current.button_send_to_release_polls = 0;
        } else {
            panel_current.button_send_to_press_polls++;
        }
    }
    if (scanning_page->sides[0].flags.f.requested || scanning_page->sides[1].flags.f.requested || (current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED)) {
        panel_current.button_scan_latch = 0;
        panel_current.button_send_to_latch = 0;
    }
    if (button_bits.b.b3 == 0) {  /* Function up */
        if (panel_current.button_function_release_polls > BUTTON_DEBOUNCE_POLLS) {
            panel_current.button_function_request = 0;
            _builtin_set_imask(15);
            button_state.b.b4 = 0;
            _builtin_set_imask(saved_imask);
            panel_current.button_function_press_polls = 0;
        } else {
            panel_current.button_function_release_polls++;
        }
    } else if (panel_current.button_function_press_polls > BUTTON_FUNCTION_HOLD_POLLS) {
        if (button_state.b.b4 == 0)
            panel_current.button_function_request = 1;
        panel_current.button_function_press_polls -= 43;
    } else if (panel_current.button_function_press_polls > BUTTON_DEBOUNCE_POLLS) {
        panel_current.button_function_press_polls++;
        if (button_state.b.b4 == 0)
            panel_current.button_function_request = 1;
        _builtin_set_imask(15);
        button_state.b.b4 = 1;
        _builtin_set_imask(saved_imask);
        panel_current.button_function_release_polls = 0;
    } else {
        panel_current.button_function_press_polls++;
    }
    if ((button_state.c & BUTTON_STATE_ANY) != 0)
        tick_idle_seconds.v = 0;
    if (power_state.f.saving && (err_mdc_alarm != 0 || mdc_fan_alarm.b6)) {
        panel_check_error_code_buttons(button_bits.b);
        return;
    }
    if (button_wake_press != 0 || panel_current.display_main == PANEL_MODE_ERROR) {
        panel_current.button_scan_latch = 0;
        panel_current.button_send_to_latch = 0;
        panel_current.button_function_request = 0;
        panel_current.button_function_back_request = 0;
    } else if (((volatile char *)&mdc_mailbox)[0x487] & MDC_SENSOR_ADF_COVER_OPEN) {  /* ADF open: Scan/Stop + Send to turn the rollers */
        if ((button_state.c & BUTTON_STATE_ANY) == (BUTTON_STATE_SCAN | BUTTON_STATE_SEND_TO)) {
            panel_current.roller_turn_request = 1;
            panel_current.button_scan_latch = 0;
            panel_current.button_send_to_latch = 0;
            panel_current.button_function_request = 0;
            panel_current.button_function_back_request = 0;
        }
    }
    if (testmode_number.v != 3 && testmode_number.v != 37)
        panel_check_error_code_buttons(button_bits.b);
}


void button_handle_in_scan(void)
{
    int saved_imask;
    struct page_record *scanning_page;
    struct page_record *current_page;

    saved_imask = _builtin_get_imask();
    scanning_page = (struct page_record *)(&(*(struct page_record *)&page_records) + page_scanning.v);
    current_page = (struct page_record *)(&(*(struct page_record *)&page_records) + page_current.v);
    if (page_side_state[0].last_buf) {
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    }
    if (maint_mode_word.v & 0x5000) {  /* maintenance mode: Send to stops (error 0x78) */
        if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) == 0 || scanning_page->flags.f.scan_end != 0) {
            if (button_state.b.b6) {
                _builtin_set_imask(15);
                err_display_flags.b.b5 = 0;
                err_display_flags.b.b6 = 0;
                err_display_flags.b.b2 = 0;
                _builtin_set_imask(saved_imask);
                panel_set_display_mode(maint_mode_word.c.mode, PANEL_MODE_MAINT);
            }
            return;
        }
        if (button_state.b.b6) {
            err_raise(ERR_STOPPED);
            if (!win_params.flags.f.adf) {
                mdc_request_stop();
            }
        }
    } else {
        if (button_state.b.b7) {
            if (power_state.f.saving) {
            } else if (!selftest_startup_done) {
            } else {
                if ((job_manual_feed_flags.w & 0x8000) && button_scan_released && job_elapsed_seconds > 1) {
                    _builtin_set_imask(15);
                    job_manual_feed_flags.w |= 8;
                    _builtin_set_imask(saved_imask);
                    mdc_request_abort();
                } else if (hw_hostif_flags.f.own_hostif && button_scan_released && (job_button_pause_flags & 0x80)) {
                    _builtin_set_imask(15);
                    job_button_pause_flags = 0;
                    job_elapsed_seconds = 0;
                    _builtin_set_imask(saved_imask);
                } else if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) && button_scan_released && job_elapsed_seconds > 1) {
                    mdc_request_abort();
                } else if (main_page_loop_phase == 2 && (page_side_state[0].events & 0x80) && button_scan_released && job_elapsed_seconds > 1) {
                    mdc_request_abort();
                }
            }
            button_scan_released = 0;
        } else {
            button_scan_released = 1;
        }
        if (button_state.b.b6) {
            if (power_state.f.saving) {
            } else if (!selftest_startup_done) {
            } else {
                if (settings_manual_feed_timeout[0] && job_elapsed_seconds > 1) {
                    _builtin_set_imask(15);
                    mdc_mailbox.manual_feed_end = 1;
                    _builtin_set_imask(saved_imask);
                    if (job_manual_feed_flags.w & 0x8000) {
                        _builtin_set_imask(15);
                        if (job_manual_feed_flags.w & 0x8000) {
                            job_manual_feed_flags.w |= 0x80;
                        }
                        _builtin_set_imask(saved_imask);
                    }
                }
                /* pause after the current ADF page (error 0x24 at the page end) */
                if (hw_hostif_flags.f.own_hostif && (settings_power_switch[0] & 2) && win_params.flags.f.adf && button_send_to_wait_release.b6 == 0 && job_elapsed_seconds > 0 && job_button_pause_flags == 0) {
                    if (((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) || (current_page->flags.v & PAGE_FLAGS_LOAD_OR_FED)) && job_driver_function_confirm == 1 && job_pause_requested == 0) {
                        _builtin_set_imask(15);
                        job_button_pause_flags |= 0x8000;
                        _builtin_set_imask(saved_imask);
                    }
                }
            }

        } else {
            button_send_to_wait_release.b6 = 0;
        }
        if (button_state.b.b6 || button_state.b.b7) {
            if (!button_error_reset_pending) {
                _builtin_set_imask(15);
                button_error_reset_pending = 1;
                _builtin_set_imask(saved_imask);
            }
            if (power_state.f.saving || hw_restart_flags.f.leaving_power_save) {
                _builtin_set_imask(15);
                button_error_reset_pending = 0;
                _builtin_set_imask(saved_imask);
            }
        }
        if (button_error_reset_pending) {
            if (testmode_number.v == 3 || testmode_number.v == 37) {
            } else if (err_mdc_alarm) {
            } else if ((button_state.c & BUTTON_STATE_ANY) == 0 && selftest_startup_done) {
                _builtin_set_imask(15);
                button_error_reset_pending = 0;
                err_display_flags.b.b5 = 0;
                err_display_flags.b.b6 = 0;
                err_display_flags.b.b2 = 0;
                _builtin_set_imask(saved_imask);
                panel_set_display_mode(0, PANEL_MODE_FUNCTION);
            }
        }
    }
}
