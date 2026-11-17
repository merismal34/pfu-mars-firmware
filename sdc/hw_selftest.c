/* The selftest file: lo code (section P) and hi code (`#pragma section H`, section PH) in source order. */
#include "fw.h"

/* Initialised data (section D): the on-chip peripheral. */
volatile struct st_sci *p_sci0 = (volatile struct st_sci *)0xffff81a0;
volatile struct st_sci *p_sci1 = (volatile struct st_sci *)0xffff81b0;
volatile struct st_sci *p_sci2 = (volatile struct st_sci *)0xffff81c0;
volatile struct st_sci *p_sci3 = (volatile struct st_sci *)0xffff81d0;
struct mtu_common_regs *p_mtu = (struct mtu_common_regs *)0xffff8240;
volatile struct st_mtu0 *p_mtu0 = (volatile struct st_mtu0 *)0xffff8260;
volatile struct st_mtu1 *p_mtu1 = (volatile struct st_mtu1 *)0xffff8280;
volatile struct st_mtu1 *p_mtu2 = (volatile struct st_mtu1 *)0xffff82a0;
volatile struct st_mtu3 *p_mtu3 = (volatile struct st_mtu3 *)0xffff8200;
volatile struct st_mtu4 *p_mtu4 = (volatile struct st_mtu4 *)0xffff8200;
void *p_mtu34 = (void *)0xffff8200;
volatile struct st_intc *p_intc = (volatile struct st_intc *)0xffff8348;
volatile struct st_pfc *p_pfc = (volatile struct st_pfc *)0xffff8384;
volatile struct st_pa *p_pa = (volatile struct st_pa *)0xffff8380;
volatile struct st_pb *p_pb = (volatile struct st_pb *)0xffff8390;
void *p_pc = (void *)0xffff8392;
void *p_pd = (void *)0xffff83a0;
volatile struct st_pe *p_pe = (volatile struct st_pe *)0xffff83b0;
volatile struct st_pf *p_pf = (volatile struct st_pf *)0xffff83b3;
volatile struct st_cmt *p_cmt = (volatile struct st_cmt *)0xffff83d0;
volatile struct st_cmt0 *p_cmt0 = (volatile struct st_cmt0 *)0xffff83d2;
volatile struct st_cmt0 *p_cmt1 = (volatile struct st_cmt0 *)0xffff83d8;
struct ad_regs *p_ad = (struct ad_regs *)0xffff8420;
void *p_flash = (void *)0xffff8580;
void *p_ubc = (void *)0xffff8600;
volatile union un_wdt *p_wdt_read = (volatile union un_wdt *)0xffff8610;
volatile union un_wdt *p_wdt_write = (volatile union un_wdt *)0xffff8610;
struct power_down_view *p_sbycr = (struct power_down_view *)0xffff8614;
volatile struct st_bsc *p_bsc = (volatile struct st_bsc *)0xffff8620;
volatile struct st_dmac *p_dmac = (volatile struct st_dmac *)0xffff86b0;
void *p_dmac0[3] = {(void *)0xffff86c0, (void *)0xffff86d0, (void *)0xffff86e0};
void *p_dmac3 = (void *)0xffff86f0;
volatile struct st_dtc *p_dtc = (volatile struct st_dtc *)0xffff8700;
volatile struct st_iic *p_iic = (volatile struct st_iic *)0xffff87f0;
void *p_unused_after_iic = (void *)0xfffffff0;
unsigned short *p_vdcc_front_gamma_0 = (unsigned short *)0xc41000;
unsigned short *p_vdcc_front_gamma_1 = (unsigned short *)0xc41800;
unsigned short *p_vdcc_front_gamma_2 = (unsigned short *)0xc42000;
struct pisces_table_set p_pisces_front_tables = {
    (unsigned short *)0xc06600, (unsigned short *)0xc06000, (unsigned short *)0xc06200, (unsigned short *)0xc06400
};
unsigned short *p_pisces_front_halftone = (unsigned short *)0xc06800;
short *p_pisces_front_ram = (short *)0xc04000;
unsigned short *p_vdcc_back_gamma_0 = (unsigned short *)0xc61000;
unsigned short *p_vdcc_back_gamma_1 = (unsigned short *)0xc61800;
unsigned short *p_vdcc_back_gamma_2 = (unsigned short *)0xc62000;
struct pisces_table_set p_pisces_back_tables = {
    (unsigned short *)0xc26600, (unsigned short *)0xc26000, (unsigned short *)0xc26200, (unsigned short *)0xc26400
};
unsigned short *p_pisces_back_halftone = (unsigned short *)0xc26800;
short *p_pisces_back_ram = (short *)0xc24000;
unsigned short *p_vdcc_front_ref_0[1] = {(unsigned short *)0xc40000};
unsigned short *p_vdcc_front_ref_1[1] = {(unsigned short *)0xc48000};
unsigned short *p_vdcc_front_ref_2[1] = {(unsigned short *)0xc40000};
unsigned short *p_vdcc_back_ref_0[1] = {(unsigned short *)0xc60000};
unsigned short *p_vdcc_back_ref_1[1] = {(unsigned short *)0xc68000};
unsigned short *p_vdcc_back_ref_2[1] = {(unsigned short *)0xc60000};
unsigned short *p_vdcc_front_ref_byte_0[1] = {(unsigned short *)0xc44000};
unsigned short *p_vdcc_front_ref_byte_1[1] = {(unsigned short *)0xc4c000};
unsigned short *p_vdcc_front_ref_byte_2[1] = {(unsigned short *)0xc44000};
unsigned short *p_vdcc_back_ref_byte_0[1] = {(unsigned short *)0xc64000};
unsigned short *p_vdcc_back_ref_byte_1[1] = {(unsigned short *)0xc6c000};
unsigned short *p_vdcc_back_ref_byte_2[1] = {(unsigned short *)0xc64000};
unsigned short *p_vdcc_front_test_table = (unsigned short *)0xc4fe00;
unsigned short *p_vdcc_back_test_table = (unsigned short *)0xc6fe00;
union pisces_sdram_ptr p_pisces_front_sdram = {(struct pisces_sdram_pixel *)0x240000};
union pisces_sdram_ptr p_pisces_back_sdram = {(struct pisces_sdram_pixel *)0x260000};
volatile struct board_ctrl_regs *p_board_ctrl = (volatile struct board_ctrl_regs *)0x210000;
struct scsi_regs_dummy *p_scsi_ctrl_dummy = (struct scsi_regs_dummy *)hw_scsi_ctrl_dummy;
void *p_scsi_ctrl = (void *)0x200000;
struct vdcc_regs *p_vdcc_front[1] = {(struct vdcc_regs *)0x220000};
struct vdcc_regs *p_vdcc_back[1] = {(struct vdcc_regs *)0x230000};
struct vdcc_correction_regs *p_vdcc_front_correction[1] = {(struct vdcc_correction_regs *)0x220010};
struct vdcc_correction_regs *p_vdcc_back_correction[1] = {(struct vdcc_correction_regs *)0x230010};
struct vdcc_measure_regs *p_vdcc_measure[2] = {(struct vdcc_measure_regs *)0x220040, (struct vdcc_measure_regs *)0x230040};
struct vdcc_monitor_words *p_vdcc_front_monitor[1] = {(struct vdcc_monitor_words *)0x220060};
struct vdcc_monitor_words *p_vdcc_back_monitor[1] = {(struct vdcc_monitor_words *)0x230060};
unsigned short *p_vdcc_timing[2] = {
    (unsigned short *)0x220070,
    (unsigned short *)0x230070
};
struct vdcc_capture_regs *p_vdcc_capture[2] = {(struct vdcc_capture_regs *)0x220090, (struct vdcc_capture_regs *)0x230090};
struct vdcc_emphasis_regs *p_vdcc_emphasis[2] = {
    (struct vdcc_emphasis_regs *)0x2200b0,
    (struct vdcc_emphasis_regs *)0x2300b0
};
struct vdcc_sensor_regs *p_vdcc_front_sensor = (struct vdcc_sensor_regs *)0x2200c0;
struct vdcc_sensor_regs *p_vdcc_back_sensor = (struct vdcc_sensor_regs *)0x2300c0;
volatile struct vdcc_counter_regs *p_vdcc_front_counter[1] = {(volatile struct vdcc_counter_regs *)0x2200d0};
volatile struct vdcc_counter_regs *p_vdcc_back_counter[1] = {(volatile struct vdcc_counter_regs *)0x2300d0};
struct vdcc_bank_regs *p_vdcc_front_bank[1] = {(struct vdcc_bank_regs *)0x2200e0};
struct vdcc_bank_regs *p_vdcc_back_bank[1] = {(struct vdcc_bank_regs *)0x2300e0};
struct vdcc_gpio_regs *p_vdcc_front_gpio[1] = {(struct vdcc_gpio_regs *)0x220100};
void *p_vdcc_back_gpio[1] = {(void *)0x230100};
struct vdcc_afe_regs *p_vdcc_afe[2] = {(struct vdcc_afe_regs *)0x220108, (struct vdcc_afe_regs *)0x230108};
struct uart_regs *p_uart[12] = {
    (struct uart_regs *)0x220110,
    (struct uart_regs *)0x230110,
    (struct uart_regs *)0x220120,
    (struct uart_regs *)0x230120,
    (struct uart_regs *)0x200020,
    (struct uart_regs *)0x400020,
    (struct uart_regs *)0x200040,
    (struct uart_regs *)0x400040,
    (struct uart_regs *)0x200600,
    (struct uart_regs *)0x400600,
    (struct uart_regs *)0x200080,
    (struct uart_regs *)0x400080
};
struct usb_regs_dummy *p_usb_ctrl_dummy = (struct usb_regs_dummy *)hw_usb_ctrl_dummy;
void *p_usb_ctrl = (void *)0xc0c000;
volatile union pisces_global_reg *p_pisces[2] = {(volatile union pisces_global_reg *)0xc00000, (volatile union pisces_global_reg *)0xc20000};
volatile struct pisces_ctrl_regs *p_pisces_ctrl[2] = {(volatile struct pisces_ctrl_regs *)0xc00002, (volatile struct pisces_ctrl_regs *)0xc20002};
volatile struct pisces_mode_regs *p_pisces_mode[2] = {(volatile struct pisces_mode_regs *)0xc00010, (volatile struct pisces_mode_regs *)0xc20010};
unsigned short (*p_pisces_ycc_matrix[2])[3] = {
    (unsigned short (*)[3])0xc00020,
    (unsigned short (*)[3])0xc20020
};
volatile struct pisces_capture_regs *p_pisces_capture[2] = {
    (volatile struct pisces_capture_regs *)0xc00040,
    (volatile struct pisces_capture_regs *)0xc20040
};
volatile struct pisces_pass_source_regs *p_pisces_pass_source[2] = {
    (volatile struct pisces_pass_source_regs *)0xc00060,
    (volatile struct pisces_pass_source_regs *)0xc20060
};
volatile struct pisces_pass_dest_regs *p_pisces_pass_dest[2] = {(volatile struct pisces_pass_dest_regs *)0xc00090, (volatile struct pisces_pass_dest_regs *)0xc20090};
volatile struct pisces_output_regs *p_pisces_output[2] = {(volatile struct pisces_output_regs *)0xc000c0, (volatile struct pisces_output_regs *)0xc200c0};
unsigned short (*p_pisces_rgb_matrix[2])[3] = {
    (unsigned short (*)[3])0xc000f0,
    (unsigned short (*)[3])0xc200f0
};
volatile struct pisces_hostif_regs *p_pisces_hostif[2] = {(volatile struct pisces_hostif_regs *)0xc00110, (volatile struct pisces_hostif_regs *)0xc20110};
volatile struct pisces_sdram_ctrl_regs *p_pisces_sdram_ctrl[2] = {(volatile struct pisces_sdram_ctrl_regs *)0xc00120, (volatile struct pisces_sdram_ctrl_regs *)0xc20120};
volatile struct pisces_input_op_regs *p_pisces_input_op[2] = {
    (volatile struct pisces_input_op_regs *)0xc00130,
    (volatile struct pisces_input_op_regs *)0xc20130
};
volatile struct pisces_zoom_regs *p_pisces_zoom[2] = {
    (volatile struct pisces_zoom_regs *)0xc00140,
    (volatile struct pisces_zoom_regs *)0xc20140
};
volatile struct pisces_gray_row_regs *p_pisces_gray_row[2] = {
    (volatile struct pisces_gray_row_regs *)0xc00150,
    (volatile struct pisces_gray_row_regs *)0xc20150
};
volatile struct pisces_kernel_ctrl_regs *p_pisces_kernel_ctrl[2] = {
    (volatile struct pisces_kernel_ctrl_regs *)0xc00160,
    (volatile struct pisces_kernel_ctrl_regs *)0xc20160
};
volatile struct pisces_filter_regs *p_pisces_filter[2] = {
    (volatile struct pisces_filter_regs *)0xc00170,
    (volatile struct pisces_filter_regs *)0xc20170
};
volatile struct pisces_binarize_regs *p_pisces_binarize[2] = {
    (volatile struct pisces_binarize_regs *)0xc00190,
    (volatile struct pisces_binarize_regs *)0xc20190
};
volatile struct pisces_jpeg_ctrl_bits *p_pisces_jpeg_ctrl[2] = {
    (volatile struct pisces_jpeg_ctrl_bits *)0xc001a0,
    (volatile struct pisces_jpeg_ctrl_bits *)0xc201a0
};
volatile struct pisces_edge_regs *p_pisces_edge[2] = {(volatile struct pisces_edge_regs *)0xc001b0, (volatile struct pisces_edge_regs *)0xc201b0};
volatile struct pisces_kernel_regs *p_pisces_kernel[2] = {
    (volatile struct pisces_kernel_regs *)0xc001e0,
    (volatile struct pisces_kernel_regs *)0xc201e0
};
struct link_window_params_record *link_window_params_ptrs[1] = {(struct link_window_params_record *)link_window_front_params};
void *link_window_back_ptr = (void *)link_window_back_params;
struct link_window_mode_view *link_window_mode_ptrs[1] = {(struct link_window_mode_view *)link_window_mode_params};
unsigned char *link_z_params_ptrs[1] = {(unsigned char *)link_z_params};
struct link_global_params *link_global_params_ptr = (struct link_global_params *)(link_z_params + 16);
struct link_side_view *link_side_params_ptrs[4] = {
    (struct link_side_view *)(link_z_params + 32),
    (struct link_side_view *)link_side_back_params,
    (struct link_side_view *)0xc08000,
    (struct link_side_view *)0xc28000
};
volatile struct jpeg_regs *p_jpeg[2] = {
    (volatile struct jpeg_regs *)0xc08000,
    (volatile struct jpeg_regs *)0xc28000
};
unsigned short *p_jpeg_front_qtables[4] = {
    (unsigned short *)0xc08200,
    (unsigned short *)0xc08280,
    (unsigned short *)0xc08300,
    (unsigned short *)0xc08380
};
unsigned short *p_jpeg_back_qtables[4] = {
    (unsigned short *)0xc28200,
    (unsigned short *)0xc28280,
    (unsigned short *)0xc28300,
    (unsigned short *)0xc28380
};
unsigned short *p_jpeg_front_huffman[8] = {
    (unsigned short *)0xc08400,
    (unsigned short *)0xc08420,
    (unsigned short *)0xc08440,
    (unsigned short *)0xc08460,
    (unsigned short *)0xc08600,
    (unsigned short *)0xc08620,
    (unsigned short *)0xc08640,
    (unsigned short *)0xc08660
};
unsigned short *p_jpeg_back_huffman[8] = {
    (unsigned short *)0xc28400,
    (unsigned short *)0xc28420,
    (unsigned short *)0xc28440,
    (unsigned short *)0xc28460,
    (unsigned short *)0xc28600,
    (unsigned short *)0xc28620,
    (unsigned short *)0xc28640,
    (unsigned short *)0xc28660
};

#pragma section H
extern void _builtin_set_imask(int);
struct board_ctrl_regs_selftest {
    struct { unsigned short p:5, f:3, :8; } variant;
    char pad_02[10];
    struct {
        unsigned short p:4,
                       b11:1,
                       cga_reset:1,
                       :1,
                       cga_select:1,
                       :8;
    } reg_0c;
    struct { unsigned short p:7, ccd_300dpi_mode:1, :8; } reg_0e;
};

int selftest_run_power_on(void)
{
    int i;
    int polls;
    unsigned char code = 0;
    int retries = 0;
    unsigned short result;
    unsigned char tries = 0;

    _builtin_set_imask(0);
    selftest_passed = 1;
    if (power_mdc_restart_state.f.retry_state == 2) {
        return 1;
    }
    polls = 0xbbd;
    if (hw_after_update != 0) {
        hw_after_update = 0;
        selftest_startup_done = 1;
        if (hw_port_b->DR.BIT.B9 == 0) {
            hw_port_a->DR.BIT.B9 = 0;
            for (i = 0; i < 200; i++) {
                rt_delay_60k();
            }
        } else {
            hw_port_a->DR.BIT.B9 = 0;
            mdc_wait_pb9_low();
        }
        mdc_request_restart();
    } else {
        if (hw_restart_flags.f.host_reset || hw_restart_flags.f.usb_restart) {
            if (selftest_mdc_in_boot) {
                return 1;
            } else {
                mdc_request_restart();
            }
        }
    }
    for (i = 0; i < polls; i++) {
        if ((unsigned short)(mdc_mailbox.handshake_low.v | (mdc_mailbox.handshake_high << 8)) == MDC_HANDSHAKE_MDC_UP) {
            break;
        }
        rt_delay_60k();
        if (i >= polls - 5) {
            return 0;
        }
    }
    util_mem_copy_to_odd((void *)&mdc_settings, settings, 0x200);
    mdc_mailbox.board_variant = ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->variant.f;
    mdc_mailbox.handshake_high = MDC_HANDSHAKE_SDC_ANSWER >> 8;   /* MDC handshake word 0: it shows 0x5555, the SDC answers 0xAAAA, the MDC 0x3377 */
    mdc_mailbox.handshake_low.v = MDC_HANDSHAKE_SDC_ANSWER & 0xff;
    polls = 0xbbd;
    for (i = 0; i < polls; i++) {
        if ((unsigned short)(mdc_mailbox.handshake_low.v | (mdc_mailbox.handshake_high << 8)) == MDC_HANDSHAKE_DONE) {
            break;
        }
        rt_delay_60k();
        if (i >= polls - 5) {
            return 0;
        }
        if ((unsigned short)(mdc_mailbox.handshake_low.v | (mdc_mailbox.handshake_high << 8)) == MDC_HANDSHAKE_MDC_UP) {
            mdc_mailbox.handshake_high = MDC_HANDSHAKE_SDC_ANSWER >> 8;
            mdc_mailbox.handshake_low.v = MDC_HANDSHAKE_SDC_ANSWER & 0xff;
        }
    }
    mdc_mailbox.startup_request = MDC_STARTUP_INIT_MECHANICS;
    if (!hw_restart_flags.f.host_reset && !hw_restart_flags.f.usb_restart) {
        code = selftest_vdcc_memories();
        if (code) {
            err_raise(code);
            selftest_show_error(0x86);
        }
        selftest_restart_uart();
        selftest_init_tone_curves();
        tries = 0;
        while (tries < 5) {
            result = selftest_pisces_indirect();
            if (result == 0) {
                break;
            } else {
                for (i = 0; i <= 200; i++) {
                    rt_delay_60k();
                }
                tries++;
            }
        }
        if (result != 0) {
            code = selftest_map_error(result);
            err_raise(code);
            selftest_show_error(0x82);
        }
    }
    polls = 0xbbd;
    for (i = 0; i < polls; i++) {
        win_params.flags.f.adf = 0;
        afe_write_config(0);
        win_params.flags.f.adf = 1;
        afe_write_config(0);
        afe_write_config(1);
        if (mdc_mailbox.startup_state == MDC_STARTUP_INIT_MECHANICS) {
            break;
        }
        rt_delay_60k();
        if (i >= polls - 5) {
            return 0;
        }
    }
    mdc_mailbox.startup_request = MDC_STARTUP_RUN;
    polls = 0xbbd;
    for (i = 0; i < polls; i++) {
        if (mdc_mailbox.startup_state == MDC_STARTUP_RUN) {
            break;
        }
        rt_delay_60k();
        if (i >= polls - 5) {
            return 0;
        }
        if (mdc_mailbox.error_code.v) {
            if (mdc_mailbox.error_code.v == ERR_ADF_COVER_OPEN) {
                if (retries++ >= 5) {
                    break;
                }
            }
            if (mdc_mailbox.error_code.v == ERR_IMPRINTER_SYSTEM) {
                selftest_show_error(0x8a);
            } else if (mdc_mailbox.error_code.v == ERR_CGA_BOARD) {
                err_raise(ERR_CGA_BOARD);
                err_pending_flags.f.cga_board = 1;
                selftest_show_error(0x8b);
            }
            err_check_pending();
        }
    }
    selftest_startup_buttons = button_state.c;
    selftest_startup_sensors = mdc_mailbox.sensor_status.v;
    if (mdc_version[0] == MDC_VERSION_BOOT) {
        selftest_mdc_in_boot = 1;
    } else {
        selftest_mdc_in_boot = 0;
    }
    if (mdc_mailbox.option_present == MDC_OPTION_PRESENT) {
        hw_option_flags.f.b0 = 1;
    }
    if (main_startup_flags.b2 == 0) {
        if (mdc_mailbox.df_adjust_flags & MDC_DF_ADJUST_DF_OK) {
            settings_df_sensor_range[0] = mdc_settings_8d[0];
            settings_df_sensor_level[0] = mdc_settings_8e[0];
        }
    }
    _builtin_set_imask(15);
    tick_mdc_start_ticks = 0;
    job_calibrated_since_mdc_start = 0;
    _builtin_set_imask(0);
    scsi_imprinter_slot = 0x201;
    if ((settings_intelligent_multifeed[0] & 0x80) && iic_eeprom_area2[0] != 0xff && iic_eeprom_area2_byte_1[0] != 0xff && iic_eeprom_area2_byte_2[0] != 0xff) {
        util_mem_copy_to_odd((void *)&mdc_back_window_overlap_patterns, iic_eeprom_area2, 0x88);
        mdc_run_write_overlap_patterns();
    }
    return 1;
}

void selftest_show_error(unsigned char code)
{
    unsigned short count;
    unsigned short flag = 1;
    char glyph;

    switch (code) {
    case 0x82:
        glyph = panel_glyph_9_pattern[0];
        break;
    case 0x86:
        glyph = panel_glyphs[0];
        break;
    case 0x87:
        glyph = panel_glyph_6_pattern[0];
        break;
    case 0x8a:
        glyph = panel_glyph_6_pattern[0];
        break;
    case 0x8b:
        glyph = panel_glyph_5_pattern[0];
        break;
    case 0x88:
        glyph = panel_glyphs[0];
        break;
    case 0x89:
        glyph = panel_glyph_8_pattern[0];
        err_last_code.s.v = ERR_MDC_TIMEOUT;
        break;
    }
    hw_port_e->DRL.BIT.B15 = 1;
    hw_port_a->DR.BIT.B1 = 1;
    hw_port_a->DR.BIT.B0 = 1;
    hw_port_e->DRL.BIT.B2 = 0;
    p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off = 1;
    p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 1;
    hw_port_e->DRL.BIT.B7 = 0;
    hw_port_e->DRL.BIT.B10 = 0;
    _builtin_set_imask(0);
    hw_port_e->DRL.BIT.B8 = 1;
    hw_port_e->DRL.BIT.B9 = 0;
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    p_pisces[0]->n.b0 = 1;
    p_pisces[1]->n.b0 = 1;
    hw_enable_vdcc(0);
    for (count = 600; count > 0; count--) {
        rt_delay_60k();
    }
    selftest_passed = 0;
    while (1) {
        if (panel_error_code_shown != 0) {
            continue;
        }
        switch (code) {
        case 0x82:
            iic_panel_set_digit(panel_glyph_e_pattern[0]);
            break;
        case 0x86:
            iic_panel_set_digit(panel_glyph_c_pattern[0]);
            break;
        case 0x87:
            iic_panel_set_digit(panel_glyph_c_pattern[0]);
            break;
        case 0x8a:
        case 0x8b:
            iic_panel_set_digit(panel_glyph_h_pattern[0]);
            break;
        case 0x88:
            iic_panel_set_digit(panel_glyph_c_pattern[0]);
            break;
        case 0x89:
            iic_panel_set_digit(panel_glyph_c_pattern[0]);
            break;
        }
        for (count = 250; count > 0; count--) {
            rt_delay_24k();
        }
        iic_panel_set_digit(panel_glyph_blank_pattern[0]);
        for (count = 250; count > 0; count--) {
            rt_delay_24k();
        }
        iic_panel_set_digit(glyph);
        for (count = 250; count > 0; count--) {
            rt_delay_24k();
        }
        if (flag == 1) {
            iic_panel_set_digit(panel_glyph_blank_pattern[0]);
            for (count = 250; count > 0; count--) {
                rt_delay_24k();
            }
        } else {
        }
    }
}

void selftest_load_settings(void)
{
    int x;

    if (!hw_restart_flags.f.leaving_power_save && !hw_restart_flags.v) {
        x = iic_eeprom_load_settings();
        settings_byte_0x9a[0] &= 0xfe;
        settings_byte_0x58[0] = 0;
    }
}

union byte_bit7_low7 {
    unsigned char c;
    struct { unsigned char b7:1, v:7; } f;
};
struct vdcc_gpio_regs_4_5 { char reg_100_103[4]; union byte_bit7_low7 reg_104, reg_105; };
struct uart_regs_selftest {
    char pad_00[3];
    struct Bits ier;
    char pad_04[5];
    struct Bits mcr;
};
union view_hw_port_f_selftest {
    struct Bits b;
    struct {
        unsigned char :2,
                      scsi_id_switch:4,
                      c:1,
                      :1;
    } f;
};


/* PE4 low after the board-control pulses: the CGA board is fitted and drives the scan (hw_hostif_flags bit 1 cleared). */
void selftest_probe_cga_board(void)
{
    int present;
    unsigned char board_type;
    unsigned char scsi_id;

    present = 0;
    ((struct uart_regs_selftest *)p_uart[0])->mcr.b0 = 0;
    ((struct uart_regs_selftest *)p_uart[0])->mcr.b1 = 0;
    if (hw_restart_flags.f.host_reset && hw_hostif_flags.f.owner_decided && hw_hostif_flags.f.own_hostif) {
    } else {
        if (hw_restart_flags.f.usb_restart) {
        } else {
            ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.cga_select = 1;
            rt_delay_180k();
            rt_delay_180k();
            if (hw_board_variant == 0) {
                ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.b11 = 1;
            }
            ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.cga_reset = 1;
            ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.cga_reset = 0;
            if (hw_port_e->DRL.BIT.B4 == 0) {
                hw_cga_board_present.f.present = 1;
                present = 1;
            } else {
                if (hw_board_variant == 0) {
                    ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.b11 = 1;
                }
                ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.cga_reset = 1;
                ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.cga_select = 0;
            }
        }
    }
    if (present) {
        hw_hostif_flags.f.own_hostif = 0;
        win_params.side[0].size_rounding.v = 0x100;
        board_type = p_vdcc_back_sensor->source_gate.f.lo;
        hw_cga_board_type = board_type;
        hw_hostif_flags.f.cga_test_scan = 1;
        hw_cga_hostif_flags.v = hw_hostif_flags.v;
        hw_hostif_flags.f.cga_test_scan = 0;
        scsi_id = hw_read_scsi_id();
        if (scsi_id == 0) {
            scsi_id = 5;
        }
        if (((union view_hw_port_f_selftest *)hw_port_f)->f.scsi_id_switch == 6) {
            scsi_id = 9;
        }
        settings_cga_scsi_id[0] = scsi_id;
        settings_setup_flags.b7 = 0;
    } else {
        hw_hostif_flags.f.own_hostif = 1;
        win_params.side[0].size_rounding.v = 0x6002;
        ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.b7 = 0;
    }
}

void selftest_wait_pf7_high(void)
{
    int i;
    int j;

    for (i = 0; i < 600; i++) {
        for (j = 0; j < 10000; j++) {
            if (!((union view_hw_port_f_selftest *)hw_port_f)->b.b7) {
            } else {
                rt_delay_60k();
                rt_delay_60k();
                if (((union view_hw_port_f_selftest *)hw_port_f)->b.b7) {
                    return;
                } else {
                    continue;
                }
            }
        }
    }
}

void selftest_clear_option_flags(void)
{
    hw_option_flags.f.b1 = 0;
    hw_option_flags.f.b0 = 0;
}


void selftest_set_mode_defaults(void)
{
    int x;

    job_compute_power_off_limit();
    if (hw_restart_flags.f.host_reset == 0 && hw_restart_flags.f.usb_restart == 0) {
        _builtin_set_imask(15);
        power_off_idle_seconds = 0;
        _builtin_set_imask(0);
    }
    _builtin_set_imask(15);
    power_off_flags.f.enabled = 1;
    power_off_flags.f.requested = 0;
    power_off_flags.f.idle_restarted = 1;
    power_off_flags.f.warning_due = 0;
    _builtin_set_imask(0);
    job_apply_settings();
    job_mode_params[0].max_burst_size = 0;
    job_mode_params[0].measurement_unit = 0;
    job_mode_params[0].unit_divisor = 0x4b0;
    job_set_sleep_timer(settings_sleep_timer_default[0], 0);
    job_set_interlace_read(0, 0);
    job_set_device_controls(0, 0);
    job_set_backing(settings_mode_page_defaults[0], 0);
    _builtin_set_imask(15);
    job_set_buffer_mode(0, 0, 0, 0);
    job_aborted = 0;
    _builtin_set_imask(0);
    job_set_auto_length(0, 0);
    job_set_deskew_and_auto_width(0, 0, 0);
    job_mode_params[0].lamp_off_minutes = settings_lamp_timer_default[0];
    if (job_mode_params[0].lamp_off_minutes == 0) {
        mdc_mailbox.lamp_off_minutes = 30;
    } else {
        mdc_mailbox.lamp_off_minutes = job_mode_params[0].lamp_off_minutes;
    }
    job_mode_params[0].job_separation[0] = 0;
}

void selftest_wait_power_button(void)
{
    int enabled;
    unsigned short presses;
    unsigned char button_bits;

    enabled = hw_power_switch_enabled();
    if (enabled) {
        if (selftest_power_button_seen == 0) {
            presses = 0;
            while (1) {
                hw_port_a->DR.BIT.B8 = 0;
                rt_delay_24k();
                rt_delay_24k();
                rt_delay_24k();
                rt_delay_24k();
                button_bits = iic_panel_read_buttons();
                if (button_bits & PANEL_BUTTON_POWER) {
                    presses++;
                    if (presses > 9) {
                        hw_port_a->DR.BIT.B8 = 1;
                        break;
                    }
                }
            }
        }
    }
    button_power_latch.v = 0;
    power_off_running = 0;
}
#pragma section

   /* merge: declared before its use */
extern void _builtin_set_imask(int);

/* hw_port_f: RAM copy of the port F pointer (hw_port_a..hw_port_e are in fw.h) */

/* Constants (section C). */
const unsigned char scsi_revision[1] = {0x30};
const unsigned char scsi_revision_tail[3] = {0x50, 0x30, 0x30};

/* Uninitialised data (section B). */
int selftest_power_button_seen;
int selftest_uart_restart_pending;

void hw_restart(void)
{
    hw_init_ram_sections();
    hw_set_vbr();
    hw_init();
    hw_startup_sequence();
    main_loop();
}

/* The start-up after the RAM init: window defaults, defect lists, self test, start-up end. */
void hw_startup_sequence(void)
{
    selftest_init_window();
    dirt_check_defect_lists();
    selftest_run_startup();
    selftest_finish_startup();
}

void hw_init(void)
{
    unsigned char code;
    code = 0;
    if (hw_init_ports() == 0) {
        code = err_last_code.s.v;
    }
    hw_init_ad();
    hw_stop_wdt();
    _builtin_set_imask(15);
    if (hw_restart_flags.f.host_reset || hw_restart_flags.f.usb_restart) {
        hw_init_intc(2);
    } else {
        hw_init_intc(0);
    }
    hw_clear_dtc();
    hw_stop_dmac();
    if (hw_restart_flags.f.host_reset) {
    } else {
        hw_init_mtu();
    }
    hw_init_cmt();
    _builtin_set_imask(0);
    if (code) {
    } else {
        hw_enable_lsi_events();
        selftest_probe_cga_board();
        hw_init_sci();
        selftest_set_mode_defaults();
        if (!hw_restart_flags.f.host_reset && !hw_restart_flags.f.usb_restart) {
            hw_check_update_signature();
        }
        if (hw_hostif_flags.f.own_hostif || hw_restart_flags.f.unused || (!hw_hostif_flags.f.selected && hw_cga_board_present.f.present)) {
            if (hw_restart_flags.f.host_reset) {
                return;
            }
            if (hw_hostif_flags.f.selected && hw_hostif_flags.f.usb) {
                usb_init();
            } else {
                if (hw_init_scsi_ctrl() == 0 && !(settings_byte_0x7f[0] & 0x80)) {
                    code = 0xef;
                }
                if (hw_init_usb_ctrl() == 0) {
                    code = 0xf4;
                }
            }
            hw_select_hostif();
            if (!hw_hostif_flags.f.selected && hw_cga_board_present.f.present) {
                hw_init_uart();
                if (hw_after_update == 0 && !hw_restart_flags.f.usb_restart) {
                    _builtin_set_imask(15);
                    tick_motor_test_ticks.v = 0;
                    _builtin_set_imask(0);
                    selftest_uart_restart_pending = 1;
                } else {
                    selftest_enable_uart_rx();
                }
            }
        } else {
            hw_init_uart();
            selftest_enable_uart_rx();
        }
    }
    if (code != 0) {
        err_raise(code);
        if (code == 0xe4) {
            selftest_show_error(0x82);
        } else if (code == 0xef || code == 0xf4) {
            selftest_show_error(0x87);
        } else if (code == 0xe9 || code == 0xea) {
            selftest_show_error(0x88);
        } else if (code == 0xf0) {
            selftest_show_error(0x89);
        } else {
            selftest_show_error(0x86);
        }
    }
}

int hw_init_ports(void)
{
    int k;
    unsigned short i;
    unsigned char button_bits;
    /* MSTCR1/2: all modules on except SCI2, SCI3 */
    p_sbycr->mstcr = 0xf01cc0c0;
    if (!hw_restart_flags.f.leaving_power_save || hw_restart_flags.f.host_reset) {
        hw_port_a = (volatile struct st_pa *)&p_pa->DR;
        hw_port_b = (volatile struct st_pb *)&p_pb->DR;
        hw_port_e = (volatile struct st_pe *)&p_pe->DRL;
        hw_port_f = (volatile struct st_pf *)&p_pf->DR;
    }
    if (!hw_restart_flags.f.leaving_power_save && !hw_restart_flags.f.host_reset && !hw_restart_flags.f.usb_restart) {
        _builtin_set_imask(0);
        if (hw_after_update) {
            panel_init_expander();
        } else {
            panel_init_expander_dark();
        }
        if (p_board_ctrl->variant.code == 3 || p_board_ctrl->variant.code == 0) {
            hw_board_variant = 0;
        } else {
            hw_board_variant = 1;
        }
        if (hw_board_variant && hw_after_update == 0) {
            selftest_power_button_seen = 0;
            for (i = 0; i < 10; i++) {
                button_bits = iic_panel_read_buttons();
                if (button_bits & PANEL_BUTTON_POWER) {
                    selftest_power_button_seen = 1;
                    panel_expander_reset_outputs();
                    break;
                }
                rt_delay_24k();
                rt_delay_24k();
                rt_delay_24k();
                rt_delay_24k();
            }
        }
    }
    selftest_load_settings();
    if (hw_board_variant && hw_after_update == 0 && !hw_restart_flags.f.leaving_power_save && !hw_restart_flags.f.host_reset && !hw_restart_flags.f.usb_restart) {
        selftest_wait_power_button();
        panel_expander_reset_outputs();
        p_board_ctrl->reg_0c.b11 = 1;
        hw_port_b->DR.BIT.B7 = 1;
    }
    p_board_ctrl->reg_02.hostif_idle = 0;
    p_board_ctrl->reg_02.standby = 0;
    p_board_ctrl->reg_04.hostif_idle_enable = 0;
    if (hw_restart_flags.f.host_reset) {
        selftest_probe_cga_board();
        panel_init_expander();
        hw_init_intc(1);
        if (hw_board_variant) {
            scsi_ctrl = p_scsi_ctrl;
            hw_init_scsi_ctrl();
        } else {
            hw_init_scsi_ctrl();
            hw_init_usb_ctrl();
        }
        hw_init_mtu();
    }
    p_board_ctrl->reg_0a.power_enable_8 = 1;
    p_board_ctrl->reg_0a.power_enable_9 = 1;
    p_board_ctrl->reg_02.hostif_idle = 0;
    p_board_ctrl->reg_02.standby = 0;
    if (hw_board_variant) {
        p_board_ctrl->reg_08.b11 = 0;
        hw_port_e->DRL.BIT.B12 = 0;
    }
    rt_delay_60k();
    p_board_ctrl->reg_06.mdc_enable = 1;
    for (k = 0; k <= 19; k++) {
        rt_delay_60k();
    }
    if (hw_board_variant) {
        p_board_ctrl->reg_0c.b11 = 0;
        hw_port_b->DR.BIT.B7 = 0;
    }
    if (hw_init_vdcc() == 0) {
        panel_expander_reset_outputs();
        err_last_code.s.v = ERR_VDCC;
        return 0;
    }
    if (hw_init_pisces() == 0) {
        panel_expander_reset_outputs();
        err_last_code.s.v = ERR_PISCES;
        return 0;
    }
    p_board_ctrl->reg_08.b10 = 0;
    if (hw_board_variant) {
        if (hw_restart_flags.f.leaving_power_save || hw_restart_flags.f.host_reset || hw_restart_flags.f.usb_restart) {
            isr_init_scsi_ctrl();
        } else {
            scsi_ctrl = p_scsi_ctrl;
            usb_ctrl = p_usb_ctrl;
        }
    } else {
        scsi_ctrl = p_scsi_ctrl;
        usb_ctrl = p_usb_ctrl;
    }
    hw_port_a->DR.BIT.B0 = 0;
    hw_port_e->DRL.BIT.B2 = 0;
    hw_port_a->DR.BIT.B0 = 1;
    hw_port_e->DRL.BIT.B7 = 0;
    hw_port_e->DRL.BIT.B10 = 0;
    if (!hw_restart_flags.f.leaving_power_save && !hw_restart_flags.f.host_reset && !hw_restart_flags.f.usb_restart && hw_after_update == 0) {
        hw_port_e->DRL.BIT.B3 = 1;
        if (hw_board_variant == 0) {
            p_vdcc_front_gpio[0]->reg_105.b7 = 0;
            if (hw_power_switch_enabled() != 0) {
                if (hw_wait_mdc_ready() != 0) {
                    return 0;
                }
            } else {
                panel_expander_reset_outputs();
            }
        }
        p_vdcc_front_gpio[0]->reg_105.b7 = 1;
    } else {
        err_display_flags.v = 0;
        err_display_flags.b.b3 = 1;
        iic_panel_set_port0(PANEL_LED_POWER);
    }
    hw_port_e->DRL.BIT.B3 = 0;
    if (hw_board_variant == 0) {
        p_board_ctrl->reg_08.b11 = 0;
    }
    for (k = 0; k <= 5; k++) {
        rt_delay_240();
    }
    hw_port_e->DRL.BIT.B9 = 1;
    for (k = 0; k <= 5; k++) {
        rt_delay_2400();
    }
    if (hw_board_variant) {
        p_pfc->PEIORL.WORD = 0x97ac;
    } else {
        p_pfc->PEIORL.WORD = 0x87ac;
    }
    hw_port_e->DRL.BIT.B8 = 0;
    selftest_wait_pf7_high();
    hw_port_a->DR.BIT.B1 = 0;
    if (hw_restart_flags.f.leaving_power_save) {
        win_params.flags.f.adf = 1;
        afe_select_source();
    }
    return 1;
}

void hw_init_bus(void)
{
    int alt_board;
    /* MSTCR1/2: all modules on except SCI2, SCI3 */
    p_sbycr->mstcr = 0xf01cc0c0;
    /* CS0-CS3 16 bit; WCR1: waits CS3 5, CS2 5, CS1 1, CS0 5 */
    p_bsc->BCR1.WORD = 0x600f;
    p_bsc->BCR2.WORD = 0x91;
    p_bsc->WCR1.WORD = 0x5515;
    p_bsc->WCR2.WORD = 0;
    p_pfc->PACRH.WORD = 0;
    /* PA10-PA15: CS0, CS1, WRL, WRH, RD, CK; PA6/PA7 CS2/CS3 in PACRL2 */
    p_pfc->PACRL1.WORD = 0x5550;
    p_pfc->PACRL2.WORD = 0xac70;
    p_pfc->PAIORH.WORD = 0;
    p_pfc->PBCR1.WORD = 0xc03;
    p_pfc->PBCR2.WORD = 0x2505;
    p_pfc->PBIOR.WORD = 0x80;
    p_pfc->PCCR.WORD = 0xffff;
    p_pfc->PCIOR.WORD = 0;
    p_pfc->PDCRH1.WORD = 0;
    p_pfc->PDCRH2.WORD = 0;
    p_pfc->PDCRL1.WORD = 0xffff;
    p_pfc->PDCRL2.WORD = 0;
    p_pfc->PDIORH.WORD = 0;
    p_pfc->PDIORL.WORD = 0;
    hw_port_a = (volatile struct st_pa *)&p_pa->DR;
    /* board variant: p_board_ctrl +0 bits 8-10, readable once PA10 is CS0 */
    if (p_board_ctrl->variant.code == 3 || p_board_ctrl->variant.code == 0) {
        alt_board = 0;
    } else {
        alt_board = 1;
    }
    if (alt_board) {
        p_pfc->PAIORL.WORD = 0x303;
        hw_port_a->DR.BIT.B8 = 1;
    } else {
        p_pfc->PAIORL.WORD = 0x203;
    }
    hw_port_a->DR.BIT.B1 = 1;
    hw_port_a->DR.BIT.B9 = 1;
    (hw_port_e = p_pe)->DRL.WORD = 0x1000;
    if (1) ;
    p_pfc->PECRL1.WORD = 0;
    p_pfc->PECRL2.WORD = 0;
    if (alt_board) {
        p_pfc->PEIORL.WORD = 0x96ac;
    } else {
        p_pfc->PEIORL.WORD = 0x86ac;
    }
    p_board_ctrl->reg_08.b10 = 1;
    if (!alt_board) {
        p_board_ctrl->reg_08.b11 = 1;
    }
    p_board_ctrl->reg_08.b8 = 1;
    p_board_ctrl->reg_08.b9 = 1;
    p_board_ctrl->reg_06.b9 = 1;
    p_board_ctrl->reg_06.b8 = 1;
    p_board_ctrl->reg_0e.ccd_300dpi_mode = 0;
}

void hw_stop_wdt(void)
{
    char x;
    x = p_wdt_read->READ.TCSR.BYTE;
    p_wdt_write->WRITE.TCSR = 0xa500;
    x = p_wdt_read->READ.RSTCSR.BYTE;
    p_wdt_write->WRITE.RSTCSR = 0xa500;
    p_wdt_write->WRITE.RSTCSR = 0x5a00;
}

void hw_clear_dtc(void)
{
    if (p_dtc->DTEA.BYTE) {
        p_dtc->DTEA.BYTE = 0;
    }
    if (p_dtc->DTEB.BYTE) {
        p_dtc->DTEB.BYTE = 0;
    }
    if (p_dtc->DTEC.BYTE) {
        p_dtc->DTEC.BYTE = 0;
    }
    if (p_dtc->DTED.BYTE) {
        p_dtc->DTED.BYTE = 0;
    }
    if (p_dtc->DTEE.BYTE) {
        p_dtc->DTEE.BYTE = 0;
    }
    if (p_dtc->DTCSR.BIT.NMIF) {
        p_dtc->DTCSR.BIT.NMIF = 0;
    }
    if (p_dtc->DTCSR.BIT.AE) {
        p_dtc->DTCSR.BIT.AE = 0;
    }
    if (p_dtc->DTCSR.BIT.SWDTE) {
        p_dtc->DTCSR.BIT.SWDTE = 0;
    }
}

void hw_stop_dmac(void)
{
    p_dmac->DMAOR.WORD = 0;
}

extern int _builtin_get_imask(void);

void hw_init_sci(void)
{
    p_sci0->SCR.BYTE = 0;
    p_sci1->SCR.BYTE = 0;
    p_sci1->SMR.BIT._PE = 1;
    p_sci1->SMR.BIT.OE = 1;
    p_sci1->SCR.BIT.CKE = 0;
    p_sci1->SCR.BIT.RE = 1;
    p_sci1->SCR.BIT.RIE = 1;
    p_sci1->SCR.BIT.TIE = 0;
    p_sci1->SCR.BIT.TEIE = 0;
    p_sci2->SCR.BYTE = 0;
    p_sci3->SCR.BYTE = 0;
    hw_set_sci1_baud();
    p_intc->IPRF.WORD |= 0xf;
}

void hw_set_sci1_baud(void)
{
    int saved_imask;
    int index;
    volatile unsigned char smr;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    p_sci1->SMR.BIT.CKS = 0;
    smr = p_sci1->SMR.BYTE;
    index = 0;
    /* console_sci1_baud[0] = {CKS, BRR} */
    smr |= console_sci1_baud[index].cks;
    p_sci1->SMR.BYTE = smr;
    p_sci1->BRR = console_sci1_baud[index].brr;
    _builtin_set_imask(saved_imask);
}

/* p_mtu points to TSTR/TSYR (0xffff8240), a block 7144s.h Ver 2.1 has only inside st_mtu (0xffff820a) */

void hw_init_mtu(void)
{
    *(unsigned char *)p_mtu = 0;
    p_mtu->tsyr = 0;
    p_mtu0->TCR.BYTE = 0;
    p_mtu0->TCR.BIT.CCLR = 0;
    p_mtu0->TCR.BIT.TPSC = 2;
    p_mtu0->TMDR.BYTE = 0;
    p_mtu0->TIOR.WORD = 0;
    p_mtu0->TIER.BYTE = 0;
    if (p_mtu0->TSR.BYTE & ~0xc0) {
        p_mtu0->TSR.BYTE = 0xc0;
    }
    p_mtu0->TCNT = 0;
    p_mtu0->TGRA = 0;
    p_mtu0->TGRB = 0;
    p_mtu0->TGRC = 0;
    p_mtu0->TGRD = 0;

    p_mtu1->TCR.BYTE = 0;
    p_mtu1->TCR.BIT.CCLR = 1;
    p_mtu1->TCR.BIT.TPSC = 3;
    p_mtu1->TMDR.BYTE = 0;
    p_mtu1->TIOR.BYTE = 0;
    p_mtu1->TIER.BYTE = 0;
    if (p_mtu1->TSR.BYTE & ~0xc0) {
        p_mtu1->TSR.BYTE = 0;
    }
    p_mtu1->TCNT = 0;
    p_mtu1->TGRA = 0x2ee;
    p_mtu1->TGRB = 0;
    p_mtu->tstr.b1 = 1;
    p_mtu1->TIER.BYTE |= 1;

    p_mtu2->TCR.BYTE = 0;
    p_mtu2->TCR.BIT.CCLR = 1;
    p_mtu2->TCR.BIT.TPSC = 3;
    p_mtu2->TMDR.BYTE = 0;
    p_mtu2->TIOR.BYTE = 0;
    p_mtu2->TIER.BYTE = 0;
    if (p_mtu2->TSR.BYTE & ~0xc0) {
        p_mtu2->TSR.BYTE = 0;
    }
    p_mtu2->TCNT = 0;
    p_mtu2->TGRA = 0xbc;
    p_mtu2->TGRB = 0;
    p_mtu->tstr.b2 = 1;
    p_mtu2->TIER.BYTE |= 1;

    p_mtu3->TCR.BYTE = 0;
    p_mtu3->TCR.BIT.CCLR = 1;
    p_mtu3->TCR.BIT.TPSC = 3;
    p_mtu3->TMDR.BYTE = 0;
    p_mtu3->TIOR.WORD = 0;
    p_mtu3->TIER.BYTE = 0;
    if (p_mtu3->TSR.BYTE & ~0xc0) {
        p_mtu3->TSR.BYTE = 0;
    }
    p_mtu3->TCNT = 0;
    p_mtu3->TGRA = 0x5e;
    p_mtu3->TGRB = 0;
    p_mtu->tstr.b6 = 1;
    p_mtu3->TIER.BYTE |= 1;

    p_mtu4->TCR.BYTE = 0;
    p_mtu4->TCR.BIT.CCLR = 1;
    p_mtu4->TCR.BIT.TPSC = 3;
    p_mtu4->TMDR.BYTE = 0;
    p_mtu4->TIOR.WORD = 0;
    p_mtu4->TIER.BYTE = 0;
    if (p_mtu4->TSR.BYTE & ~0xc0) {
        p_mtu4->TSR.BYTE = 0;
    }
    p_mtu4->TCNT = 0;
    p_mtu4->TGRA = 0xbc;
    p_mtu4->TGRB = 0;
    p_mtu4->TGRC = 0;
    p_mtu4->TGRD = 0;
    p_mtu->tstr.b7 = 1;
    p_mtu4->TIER.BYTE |= 1;

    p_intc->IPRD.WORD = 0x20a0;
    p_intc->IPRE.WORD = 0x1020;
    p_intc->IPRF.WORD |= 0x8000;
    p_intc->IPRA.WORD |= 0xe00;
}

/* p_ad (A/D, 0xffff8420) reaches ADCR of both units (+104, +105): 7144s.h Ver 2.1 splits them into
   st_ad0 and st_ad1 (0xffff8428), so the firmware's header had one A/D block here */

void hw_init_cmt(void)
{
    p_cmt->CMSTR.WORD = 0;
    util_read16((short *)&p_cmt0->CMCSR.WORD);
    p_cmt0->CMCSR.WORD = 0;
    p_cmt0->CMCNT = 0;
    p_cmt0->CMCOR = 0;
    util_read16((short *)&p_cmt1->CMCSR.WORD);
    p_cmt1->CMCSR.WORD = 3;
    p_cmt1->CMCNT = 0;
    p_cmt1->CMCOR = 0;
}

void hw_init_ad(void)
{
    util_mem_clear(p_ad, 106);
    p_ad->adcr_0.cks = 0;
    p_ad->adcr_1.cks = 0;
}

void hw_init_intc(int param)
{
    int dummy;
    if (param == 2) {
        return;
    }
    p_intc->ICR1.WORD = 0xff;
    p_intc->ICR2.WORD = 0;
    p_intc->IPRA.WORD = 3;
    p_intc->IPRB.WORD = 0;
    p_intc->IPRC.WORD = 0;
    p_intc->IPRD.WORD = 0;
    p_intc->IPRE.WORD = 0;
    p_intc->IPRF.WORD = 0;
    p_intc->IPRG.WORD = 4;
    p_intc->IPRH.WORD = 0;
    p_intc->IPRI.WORD = 0;
    p_intc->IPRJ.WORD = 0;
    while (p_intc->ISR.WORD) {
        p_intc->ISR.WORD = 0;
    }
}

void hw_enable_lsi_events(void)
{
    _builtin_set_imask(15);
    p_vdcc_front[0]->events.c = 0xfd;
    p_vdcc_front[0]->mdc_event.mdc_result_ready = 1;
    p_vdcc_back[0]->events.c = 0xfd;
    p_vdcc_back[0]->mdc_event.mdc_result_ready = 1;
    p_intc->IPRA.WORD |= 0xc000;
    _builtin_set_imask(0);
    p_vdcc_front[0]->irq_enable.f.line_irq_enable = 1;
    p_pisces_ctrl[0]->reg_00a.v = 0;
    p_pisces_ctrl[0]->event_enable.f.b4 = 0;
    p_pisces_ctrl[0]->reg_00a.f.b4 = 1;
    p_pisces_ctrl[0]->event_enable.f.b4 = 1;
    p_pisces_ctrl[1]->reg_00a.v = 0;
    p_pisces_ctrl[1]->event_enable.f.b4 = 0;
    p_pisces_edge[0]->ctrl.enable = 1;
    p_pisces_edge[1]->ctrl.enable = 1;
}

void hw_init_ram_sections(void)
{
    unsigned int *p;
    unsigned int *q;
    for (p = hw_section_lo_b_begin; p < hw_section_lo_b_end; p++) {
        *p = 0;
    }
    for (p = hw_section_onchip_begin; p < hw_section_onchip_end; p++) {
        *p = 0;
    }
    for (p = hw_section_hi_b_begin; p < hw_section_hi_b_end; p++) {
        *p = 0;
    }
    p = hw_section_lo_d_begin;
    q = hw_section_table;
    for (; p < hw_section_lo_d_end; p++, q++) {
        *p = *q;
    }
    p = hw_section_hi_d_begin;
    q = hw_section_hi_d_rom;
    for (; p < hw_section_hi_d_end; p++, q++) {
        *p = *q;
    }
    if (power_dump_inhibit == 0) {
        for (p = hw_section_mem_begin; p < hw_section_mem_end; p++) {
            *p = 0;
        }
    }
}

extern void _builtin_set_vbr(void **);

void hw_init_ram_set3(void)
{
    unsigned int *p;
    unsigned int *q;
    int dump_inhibited;
    if (power_dump_inhibit == 0x80) {
        dump_inhibited = 1;
    } else {
        dump_inhibited = 0;
    }
    for (p = hw_section_set3_b_begin; p < hw_section_set3_b_end; p++) {
        *p = 0;
    }
    p = hw_section_set3_d_begin;
    q = hw_section_set3_d_rom;
    for (; p < hw_section_set3_d_end; p++, q++) {
        *p = *q;
    }
    if (dump_inhibited == 1) {
        power_dump_inhibit = 0xff;
    }
}

void hw_set_vbr(void)
{
    _builtin_set_vbr(hw_vbr_value);
}

int hw_power_switch_enabled(void)
{
    if (settings_power_switch[0] & 0x80) {
        return 0;
    }
    return 1;
}

int hw_wait_mdc_ready(void)
{
    unsigned char button_bits;
    unsigned char tries;
    unsigned char found;
    unsigned char held_polls;
    int polls;
    button_bits = 0;
    tries = 0;
    found = 0;
    held_polls = 0;
    polls = 0;
    button_power_latch.v = 0;
    power_off_running = 0;
    if (hw_check_mdc_signature() == 0) {
        for (;;) {
            if (tries >= 3) {
                tries = 0;
                break;
            } else {
                rt_delay_24k();
                rt_delay_24k();
                rt_delay_24k();
                rt_delay_24k();
                button_bits = iic_panel_read_buttons();
                if (button_bits & PANEL_BUTTON_POWER) {
                    if (held_polls < 255) {
                        held_polls++;
                    }
                } else if (!found) {
                    held_polls = 0;
                    found = 1;
                }
                if (!hw_port_e->DRL.BIT.B12) {
                    tries++;
                    polls = 0;
                } else {
                    tries = 0;
                    polls++;
                }
                if (polls > 0x753) {
                    /* error 0xF0: the MDC did not come up */
                    err_last_code.s.v = ERR_MDC_TIMEOUT;
                    panel_expander_reset_outputs();
                    return 1;
                }
            }
        }
    }
    p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 1;
    p_pisces_sdram_ctrl[0]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[1]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[0]->ctrl.b.b3 = 0;
    p_pisces_sdram_ctrl[1]->ctrl.b.b3 = 0;
    rt_delay_24k();
    p_pisces[0]->n.b0 = 1;
    p_pisces[1]->n.b0 = 1;
    hw_enable_vdcc(0);
    p_board_ctrl->reg_04.hostif_idle_enable = 1;
    p_board_ctrl->reg_02.hostif_idle = 1;
    p_board_ctrl->reg_02.standby = 1;
    p_board_ctrl->reg_06.mdc_enable = 0;
    p_board_ctrl->reg_0a.power_enable_8 = 0;
    p_board_ctrl->reg_0a.power_enable_9 = 0;
    hw_port_e->DRL.BIT.B8 = 1;
    for (polls = 0; polls <= 50; polls++) {
        button_bits = iic_panel_read_buttons();
        if (button_bits & PANEL_BUTTON_POWER) {
            if (held_polls < 255) {
                held_polls++;
            }
        } else if (!found) {
            held_polls = 0;
            found = 1;
        }
        rt_delay_24k();
        rt_delay_24k();
        rt_delay_24k();
        rt_delay_24k();
    }
    for (;;) {
        if (held_polls >= 10 && found) {
            break;
        } else {
            rt_delay_24k();
            rt_delay_24k();
            rt_delay_24k();
            rt_delay_24k();
            button_bits = iic_panel_read_buttons();
            if (button_bits & PANEL_BUTTON_POWER) {
                held_polls++;
            } else {
                held_polls = 0;
                found = 1;
            }
        }
    }
    hw_port_e->DRL.BIT.B8 = 0;
    p_board_ctrl->reg_0a.power_enable_8 = 1;
    p_board_ctrl->reg_0a.power_enable_9 = 1;
    p_board_ctrl->reg_02.hostif_idle = 0;
    p_board_ctrl->reg_02.standby = 0;
    p_board_ctrl->reg_04.hostif_idle_enable = 0;
    rt_delay_60k();
    p_board_ctrl->reg_06.mdc_enable = 1;
    for (polls = 0; polls <= 4; polls++) {
        rt_delay_60k();
    }
    p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 0;
    panel_expander_reset_outputs();
    for (polls = 0; polls <= 15; polls++) {
        rt_delay_60k();
    }
    hw_enable_vdcc(1);
    if (hw_init_pisces() == 0) {
        err_last_code.s.v = ERR_PISCES;
        return 1;
    }
    return 0;
}

/* True once the MDC has written its signature "BA00" to every second byte from 0xC4000B. */
unsigned char hw_check_mdc_signature(void)
{
    if (mdc_version[0] == 0x42 && mdc_signature_gap_1[0] == 0 && mdc_version_1[0] == 0x41 && mdc_signature_gap_2[0] == 0  /* even bytes of the window are not MDC RAM */
        && mdc_version_2[0] == 0x30 && mdc_signature_gap_3[0] == 0 && mdc_version_3[0] == 0x30 && mdc_signature_gap_4[0] == 0) {
        return 1;
    }
    return 0;
}

struct byte_field { unsigned char v:8; };
struct vdcc_timing_regs_selftest {
    unsigned short line_period;
    struct byte_field reg_072, reg_073, reg_074, reg_075, reg_076, reg_077;
    struct { unsigned short p:2, v:14; } reg_078, pixels_per_line;
    char reg_07c;
    struct {
        unsigned char h:4,
                      b3:1,
                      b2:1,
                      l:2;
    } reg_07d;
    unsigned short reg_07e, reg_080;
    char reg_082_083[2];
    unsigned short reg_084;
};

int hw_init_vdcc(void)
{
    unsigned int i;
    hw_enable_vdcc(1);
    if (!hw_restart_flags.f.leaving_power_save && hw_restart_flags.v == 0) {
        if (selftest_fill_verify(&p_vdcc_front_correction[0]->follow_gain_0_1.w, 4, 0xffff)) {
            return 0;
        }
        if (selftest_fill_verify(&p_vdcc_front_correction[0]->follow_gain_0_1.w, 4, 0)) {
            return 0;
        }
        if (selftest_fill_verify(&p_vdcc_back_correction[0]->follow_gain_0_1.w, 4, 0xffff)) {
            return 0;
        }
        if (selftest_fill_verify(&p_vdcc_back_correction[0]->follow_gain_0_1.w, 4, 0)) {
            return 0;
        }
    }
    p_vdcc_front[0]->reset_ctrl.soft_reset = 1;
    p_vdcc_back[0]->reset_ctrl.soft_reset = 1;
    for (;;) {
        if (p_vdcc_front[0]->reset_ctrl.soft_reset == 0 && p_vdcc_back[0]->reset_ctrl.soft_reset == 0) {
            break;
        }
    }
    hw_enable_vdcc(1);
    if (hw_restart_flags.f.leaving_power_save || hw_restart_flags.f.host_reset || hw_restart_flags.f.usb_restart) {
    } else {
        p_vdcc_front[0]->reset_ctrl.uart_reset = 1;
        p_vdcc_front[0]->reset_ctrl.b2 = 1;
        p_vdcc_back[0]->reset_ctrl.uart_reset = 1;
        p_vdcc_back[0]->reset_ctrl.b2 = 1;
        while (1) {
            if (p_vdcc_front[0]->reset_ctrl.uart_reset == 0 && p_vdcc_front[0]->reset_ctrl.b2 == 0 && p_vdcc_back[0]->reset_ctrl.uart_reset == 0
                && p_vdcc_back[0]->reset_ctrl.b2 == 0) {
                break;
            }
        }
    }
    for (i = 0; i <= 1; i++) {
        if (i == 0) {
            p_vdcc_front[i]->r0.b7 = 0;
            p_vdcc_front[i]->r0.b2 = 1;
            p_vdcc_front_gpio[i]->reg_105.b7 = 1;
        } else {
            p_vdcc_front[i]->r0.b7 = 1;
            p_vdcc_front[i]->r0.b4 = 1;
            p_vdcc_front[i]->r0.b3 = 1;
            p_vdcc_front[i]->r0.b2 = 1;
            hw_vdcc_back_gpio_copy = p_vdcc_back_gpio[0];
        }
        p_vdcc_front_sensor->reg_0c9.b7 = 0;
        p_vdcc_front_sensor->source_gate.n.n11.l = 0;
        p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off = 1;
        p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 1;
        p_vdcc_front_sensor->source_gate.n.n11.l = 15;
        p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off = 1;
        p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 1;
        p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 0;
        p_vdcc_back_sensor->reg_0c9.b7 = 0;
        p_vdcc_back_sensor->source_gate.n.n11.l = 0;
        p_vdcc_front[i]->r2.b0 = 0;
        p_vdcc_front[i]->irq_enable.v = 0;
        p_vdcc_front[i]->r9.m = 0;
        p_vdcc_front[i]->r9.l = 0;
        p_vdcc_front[i]->emphasis_ctrl.emphasis_enable = 0;
        p_vdcc_front[i]->emphasis_ctrl.l = 0;
        p_vdcc_front[i]->output_ctrl.m = 0;
        p_vdcc_front[i]->output_ctrl.gamma_enable = 0;
        if (i == 0) {
            hw_init_afe_port(i, 0);
        }
        hw_init_afe_port(i, 1);
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->line_period = 0x1382;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_073.v = 2;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_072.v = 8;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_075.v = 2;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_077.v = 2;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_076.v = 4;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_078.v = 4;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->pixels_per_line.v = 0x1f2c;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_07d.b3 = 1;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_07d.b2 = 0;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_07d.l = 0;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_07e = 0;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_080 = 0;
        ((struct vdcc_timing_regs_selftest *)p_vdcc_timing[i])->reg_084 = 0xca;
        p_vdcc_capture[i]->x_res_ctrl.b.b7 = 0;
        p_vdcc_capture[i]->capture_ctrl.output_enable = 1;
        p_vdcc_capture[i]->d.b7 = 1;
        p_vdcc_capture[i]->d.b6 = 1;
        p_vdcc_capture[i]->d.b5 = 0;
        p_vdcc_capture[i]->d.b2 = 1;
        p_vdcc_capture[i]->d.b3 = 0;
        p_vdcc_capture[i]->capture_ctrl.b5 = 1;
        p_vdcc_capture[i]->capture_ctrl.b4 = 0;
        if (settings_byte_0x33[0] & 8) {
            p_vdcc_capture[i]->capture_ctrl.b4 = 1;
        }
        p_vdcc_capture[i]->capture_ctrl.line_gate = 1;
        p_vdcc_capture[i]->f.m = 3;
        p_vdcc_capture[i]->f.l = 7;
        p_vdcc_capture[i]->e.b7 = 0;
        p_vdcc_capture[i]->e.m = 0;
        p_vdcc_capture[i]->e.b3 = 0;
        p_vdcc_capture[i]->e.l = 0;
        p_vdcc_capture[i]->reg_0a1 = 50;
        p_vdcc_capture[i]->margin_start_px = 0;
        p_vdcc_capture[i]->margin_end_px.v = 0;
        p_vdcc_capture[i]->reg_0a4.v = 0x4b0;
        p_vdcc_capture[i]->reg_0a6.v = 0x4b0;
    }
    p_vdcc_front_gpio[0]->reg_106 = 0;
    p_vdcc_front_gpio[0]->reg_107.l = 2;
    p_vdcc_front[0]->events.c = 0xfd;
    p_vdcc_front[0]->mdc_event.mdc_result_ready = 1;
    p_vdcc_back[0]->events.c = 0xfd;
    p_vdcc_back[0]->mdc_event.mdc_result_ready = 1;
    p_vdcc_front_sensor->source_gate.b.b2 = 0;
    p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    return 1;
}

void hw_enable_vdcc(int on)
{
    if (on) {
        p_vdcc_front[0]->reset_ctrl.chip_enable = 3;
        p_vdcc_front[0]->output_ctrl.output_enable = 1;
        p_vdcc_back[0]->reset_ctrl.chip_enable = 3;
        p_vdcc_back[0]->output_ctrl.output_enable = 1;
    } else {
        p_vdcc_front[0]->output_ctrl.output_enable = 0;
        p_vdcc_front[0]->reset_ctrl.chip_enable = 0;
        p_vdcc_back[0]->output_ctrl.output_enable = 0;
        p_vdcc_back[0]->reset_ctrl.chip_enable = 0;
    }
}

void hw_init_afe_port(int side, int on)
{
    if (side == 0) {
        if (on) {
            p_vdcc_afe[side]->ctrl.f.adf_select = 1;
        } else {
            p_vdcc_afe[side]->ctrl.f.adf_select = 0;
        }
    } else {
        p_vdcc_afe[side]->ctrl.f.adf_select = 0;
    }
    p_vdcc_afe[side]->port_ctrl.port_enable = 1;
    p_vdcc_afe[side]->ctrl.f.b3 = 0;
    p_vdcc_afe[side]->ctrl.f.b2 = 0;
    p_vdcc_afe[side]->ctrl.f.b1 = 0;
    p_vdcc_afe[side]->ctrl.f.busy = 0;
    p_vdcc_afe[side]->reg_109 = 32;
    p_vdcc_afe[side]->format.h = 15;
    p_vdcc_afe[side]->format.l = 7;
    p_vdcc_afe[side]->port_ctrl.port_enable = 1;
}


struct cell_u32_selftest { unsigned int v; };
int hw_init_pisces(void)
{
    unsigned int i;
    if (!hw_restart_flags.f.leaving_power_save && hw_restart_flags.v == 0) {
        if (selftest_fill_verify(&p_pisces_pass_dest[0]->y_start, 2, 0xffff)) {
            return 0;
        }
        if (selftest_fill_verify(&p_pisces_pass_dest[0]->y_start, 2, 0)) {
            return 0;
        }
        if (selftest_fill_verify(&p_pisces_pass_dest[1]->y_start, 2, 0xffff)) {
            return 0;
        }
        if (selftest_fill_verify(&p_pisces_pass_dest[1]->y_start, 2, 0)) {
            return 0;
        }
    }
    for (i = 0; i <= 1; i++) {
        p_pisces[i]->m.b = 2;
        p_pisces[i]->m.a = 2;
        p_pisces[i]->m.d = 0;
        rt_delay_24k();
        rt_delay_24k();
        p_pisces_ctrl[i]->ctrl.f.f8 = 3;
        p_pisces_mode[i]->run.v = 0;
        p_pisces_ycc_matrix[i][0][0] = 77;
        p_pisces_ycc_matrix[i][0][1] = 0x96;
        p_pisces_ycc_matrix[i][0][2] = 29;
        p_pisces_ycc_matrix[i][1][0] = 0x22b;
        p_pisces_ycc_matrix[i][1][1] = 0x255;
        p_pisces_ycc_matrix[i][1][2] = 0x80;
        p_pisces_ycc_matrix[i][2][0] = 0x80;
        p_pisces_ycc_matrix[i][2][1] = 0x26b;
        p_pisces_ycc_matrix[i][2][2] = 0x215;
        p_pisces_rgb_matrix[i][0][0] = 0x100;
        p_pisces_rgb_matrix[i][0][1] = 0;
        p_pisces_rgb_matrix[i][0][2] = 0x167;
        p_pisces_rgb_matrix[i][1][0] = 0x100;
        p_pisces_rgb_matrix[i][1][1] = 0x258;
        p_pisces_rgb_matrix[i][1][2] = 0x2b7;
        p_pisces_rgb_matrix[i][2][0] = 0x100;
        p_pisces_rgb_matrix[i][2][1] = 0x1c6;
        p_pisces_rgb_matrix[i][2][2] = 0;
        p_pisces_hostif[i]->ctrl.v = 0;
        p_pisces_hostif[i]->ctrl.f.a = 2;
        p_pisces_hostif[i]->ctrl.f.b = 3;
        (*(struct cell_u32_selftest (*)[])img_sdram_bytes)[i].v = 0x18000000;
        p_pisces_sdram_ctrl[i]->reg_122.f0 = 2;
        p_pisces_sdram_ctrl[i]->reg_122.b2 = 1;
        p_pisces_sdram_ctrl[i]->reg_122.b4 = 1;
        p_pisces_sdram_ctrl[i]->reg_122.b6 = 1;
        p_pisces_sdram_ctrl[i]->reg_122.b8 = 1;
        p_pisces_sdram_ctrl[i]->reg_122.b10 = 1;
        p_pisces_sdram_ctrl[i]->reg_122.f12 = 3;
        p_pisces_sdram_ctrl[i]->ctrl.v = 0;
        rt_delay_24k();
        p_pisces_sdram_ctrl[i]->ctrl.b.b0 = 1;
        p_pisces_sdram_ctrl[i]->reg_124 = 0x270;
        p_pisces_sdram_ctrl[i]->ctrl.b.b3 = 1;
        p_pisces_input_op[i]->input_op = PISCES_INPUT_PASS;
        img_load_pisces_preset_1(i);
        p_pisces_jpeg_ctrl[i]->b1 = 1;
    }
    return 1;
}


char selftest_vdcc_memories(void)
{
    unsigned short result;
    char code;
    char u;
    result = 0;
    result = selftest_vdcc_small_rams();
    if (result != 0) goto done;
    result = selftest_vdcc_bank_rams();
    if (result != 0) goto done;
    result = selftest_vdcc_bank_select();
    if (result != 0) goto done;
    result = selftest_pisces_rams();
    if (result != 0) goto done;
    return 0;
done:
    p_vdcc_front_bank[0]->bank = 0;
    p_vdcc_back_bank[0]->bank = 0;
    code = selftest_map_error(result);
    return code;
}

/* Constants (section C). */
const unsigned char hw_scsi_id_table[16] = {5, 5, 5, 5, 5, 5, 5, 5, 7, 6, 5, 4, 3, 2, 1, 0};
const unsigned char hw_window_mode_codes[8] = {6, 7, 0x0f, 3, 3, 0x0c, 4, 0x0d};

int hw_fill_verify_16(unsigned short *area, unsigned count, unsigned char value)
{
    unsigned i;
    unsigned short *p;
    i = 0;
    p = area;
    for (; i < count; i++, p++) {
        *p = value;
    }
    i = 0;
    p = area;
    for (; i < count; i++, p++) {
        if (*p != value) return 0;
    }
    return 1;
}

extern void _builtin_set_imask(int);
extern void _builtin_set_gbr(void *);

void hw_init_uart(void)
{
    unsigned short status;
    unsigned char baud_index;
    _builtin_set_imask(15);
    p_uart[0]->lcr.dlab = 0;
    p_uart[0]->lcr.set_break = 0;
    p_uart[0]->lcr.stick_parity = 0;
    p_uart[0]->lcr.even_parity = 0;
    p_uart[0]->lcr.parity_enable = 1;
    p_uart[0]->lcr.two_stop_bits = 0;
    p_uart[0]->lcr.word_length = 3;
    p_uart[0]->mcr.out2 = 0;
    p_uart[0]->mcr.out1 = 0;
    p_uart[0]->mcr.dtr = 0;
    p_uart[0]->mcr.rts = 1;
    if (hw_restart_flags.f.host_reset || hw_restart_flags.f.usb_restart) {
        p_uart[0]->lcr.dlab = 1;
        baud_index = uart_baud_index.v;
        p_uart[0]->rbr_thr = uart_divisor_low_bytes[baud_index * 2];
        p_uart[0]->ier = uart_divisor_table[baud_index] >> 8;
        p_uart[0]->lcr.dlab = 0;
    } else {
        baud_index = settings_uart_baud[0] & 3;
        if (baud_index == 0) {
            baud_index = 1;
        } else if (baud_index == 1) {
            baud_index = 0;
        }
        uart_baud_index.v = baud_index;
        uart_baud_index_new = baud_index;
        p_uart[0]->lcr.dlab = 1;
        p_uart[0]->rbr_thr = uart_divisor_low_bytes[baud_index * 2];
        p_uart[0]->ier = uart_divisor_table[baud_index] >> 8;
        p_uart[0]->lcr.dlab = 0;
    }
    baud_index = p_uart[0]->rbr_thr;
    status = p_uart[0]->iir_fcr;
    status = p_uart[0]->lsr;
    _builtin_set_imask(0);
}

void hw_check_update_signature(void)
{
    if (hw_after_update == 1) return;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    if (scsi_ctrl->dtbc2 == 118) {   /* 118, 84, 50: left by program 2 (fw_update_shutdown) before its restart */
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc2 = 0;
        _builtin_set_gbr(0);
        if (scsi_ctrl->dtbc1 == 84) {
            _builtin_set_gbr(0);
            scsi_ctrl->dtbc1 = 0;
            _builtin_set_gbr(0);
            if (scsi_ctrl->dtbc0 == 50) {
                _builtin_set_gbr(0);
                scsi_ctrl->dtbc0 = 0;
                _builtin_set_gbr(0);
                hw_after_update = 1;
                return;
            }
        }
    }
    hw_after_update = 0;
    _builtin_set_imask(0);
}


extern int _builtin_get_imask(void);

int hw_init_scsi_ctrl(void)
{
    int saved_imask;
    char own_id;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(saved_imask);
    if ((maint_mode_word.v & 0x5000) || (hw_hostif_flags.f.owner_decided && hw_cga_board_present.f.present)) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        scsi_ctrl->reset = 7;
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b7 = 0;
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b6 = 1;
        _builtin_set_imask(saved_imask);
        return 1;
    }
    if ((settings_byte_0x7f[0] & 0x80) == 0) {
        if (hw_port_f->b0 == 0) {
            hw_device_errors.f.scsi_interface = 1;
            if (settings_scsi_fault_count[0] != 0xff) {
                settings_scsi_fault_count[0]++;
                iic_eeprom_save_settings();
            }
        }
    }
    _builtin_set_imask(15);
    if (!hw_restart_flags.v) {
        own_id = hw_read_scsi_id();
        _builtin_set_gbr(0);
        scsi_ctrl->ownid = own_id;
        _builtin_set_gbr(0);
        hw_scsi_own_id = own_id;
        _builtin_set_gbr(0);
        scsi_own_id_bit = scsi_id_to_bit(own_id);
        _builtin_set_gbr(0);
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b7 = 0;
        _builtin_set_gbr(0);
        if (scsi_ctrl->scsimode0.b.b7) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b6 = 1;
        _builtin_set_gbr(0);
        if (scsi_ctrl->scsimode0.b.b6 == 0) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b6 = 0;
        _builtin_set_gbr(0);
        if (scsi_ctrl->scsimode0.b.b6) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc2 = 0xff;
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc1 = 0xff;
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc0 = 0xff;
        _builtin_set_gbr(0);
        if (scsi_ctrl->dtbc2 != 0xff) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        if (scsi_ctrl->dtbc1 != 0xff) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        if (scsi_ctrl->dtbc0 != 0xff) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc2 = 0;
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc1 = 0;
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc0 = 0;
        _builtin_set_gbr(0);
        if (scsi_ctrl->dtbc2 != 0) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        if (scsi_ctrl->dtbc1 != 0) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        _builtin_set_gbr(0);
        if (scsi_ctrl->dtbc0 != 0) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
    }
    _builtin_set_gbr(0);
    scsi_ctrl->reset = 7;
    _builtin_set_gbr(0);
    /* SCSIMODE0 0x43: DTCD, AN_C, AN_D; SCSIMODE1 0x81: STPPE, SPCEN; CONFIG0 0xA0: ACP, PSLV */
    scsi_ctrl->scsimode0.v = 0x43;
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode1.v = 0x81;
    _builtin_set_gbr(0);
    scsi_ctrl->ownid = hw_scsi_own_id;
    _builtin_set_gbr(0);
    scsi_ctrl->config0.b.b6 = 0;
    _builtin_set_gbr(0);
    scsi_ctrl->config0.v = 0xa0;
    _builtin_set_gbr(0);
    scsi_ctrl->config1 = 1;
    _builtin_set_gbr(0);
    own_id = scsi_ctrl->scsiint1.v;
    _builtin_set_gbr(0);
    scsi_ctrl->scsiint1.v = own_id;
    _builtin_set_gbr(0);
    own_id = scsi_ctrl->scsiint2.v;
    _builtin_set_gbr(0);
    scsi_ctrl->scsiint2.v = own_id;
    _builtin_set_gbr(0);
    own_id = scsi_ctrl->mainint.v;
    _builtin_set_gbr(0);
    scsi_ctrl->mainint.v = own_id;
    _builtin_set_gbr(0);
    scsi_ctrl->dmactl.b.b1 = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->sdid.v = 0;
    _builtin_set_gbr(0);
    scsi_ctrl->seltime = 0xa0;
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->syncmode.v = hw_restart_scsi_syncmode.v;
    if (hw_restart_scsi_ultra.v != 0) {
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b4 = 1;
    }
    scsi_win_size_rounding = 0x6002;
    if (hw_restart_flags.f.host_reset) {
        hw_select_hostif();
    }
    _builtin_set_imask(saved_imask);
    return 1;
}

int hw_read_scsi_id(void)
{
    unsigned char id;
    id = hw_scsi_id_table[((struct port_f_bits *)hw_port_f)->scsi_id_switch];   /* SCSI ID switch on port F bits 2-5 (default ID 5) */
    if (id > 7) {
        id = 5;
    }
    return (char)id;
}

void hw_unused_1(void)
{
}


int hw_init_usb_ctrl(void)
{
    int u;
    p_pisces_hostif[1]->ctrl.f.b0 = 1;
    p_pisces_hostif[1]->ctrl.f.b9 = 0;
    p_pisces_hostif[0]->ctrl.f.b0 = 1;
    p_pisces_hostif[0]->ctrl.f.b9 = 0;
    p_pisces_ctrl[0]->ctrl.f.hostif_stop = 1;
    _builtin_set_imask(0);
    if ((maint_mode_word.v & 0x5000) || (hw_hostif_flags.f.owner_decided && hw_cga_board_present.f.present)) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        usb_ctrl->chip_reset.b.b0 = 1;
        _builtin_set_gbr(0);
        usb_ctrl->main_int_enb = 0;
        _builtin_set_gbr(0);
        usb_ctrl->sie_int_enb = 0;
        _builtin_set_gbr(0);
        usb_ctrl->reg_0a = 0;
        _builtin_set_imask(0);
        return 1;
    }
    _builtin_set_imask(15);
    if (!hw_restart_flags.v) {
        _builtin_set_gbr(0);
        usb_ctrl->pm_control.b.b0 = 0;
        while (1) {
            _builtin_set_gbr(0);
            if (usb_ctrl->pm_control.b.b7 == 0) {
                break;
            }
        }
        _builtin_set_gbr(0);
        usb_ctrl->chip_reset.b.b0 = 1;
        _builtin_set_gbr(0);
        usb_ctrl->reg_17.b5 = 1;
        _builtin_set_gbr(0);
        usb_ctrl->epa_int_enb = 0xff;
        _builtin_set_gbr(0);
        if (usb_ctrl->epa_int_enb != 0xff) {
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        usb_ctrl->epa_int_enb = 0;
        _builtin_set_gbr(0);
        if (usb_ctrl->epa_int_enb != 0) {
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        /* +63: bank select for the registers from +65 up (banks 0, 1, 2) */
        usb_ctrl->usb_bank = 2;
        _builtin_set_gbr(0);
        if (usb_ctrl->usb_bank != 2) {
            _builtin_set_gbr(0);
            usb_ctrl->usb_bank = 0;
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        usb_ctrl->usb_bank = 1;
        _builtin_set_gbr(0);
        if (usb_ctrl->usb_bank != 1) {
            _builtin_set_gbr(0);
            usb_ctrl->usb_bank = 0;
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        usb_ctrl->usb_bank = 0;
        _builtin_set_gbr(0);
        if (usb_ctrl->usb_bank != 0) {
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        usb_ctrl->usb_bank = 1;
        _builtin_set_gbr(0);
        usb_ctrl->reg_20 = 16;
        _builtin_set_gbr(0);
        usb_ctrl->usb_bank = 0;
        _builtin_set_gbr(0);
        usb_ctrl->main_int_enb = 0xff;
        _builtin_set_gbr(0);
        if (usb_ctrl->main_int_enb != 0xff) {
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        usb_ctrl->main_int_enb = 0;
        _builtin_set_gbr(0);
        if (usb_ctrl->main_int_enb != 0) {
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        usb_ctrl->usb_bank = 1;
        _builtin_set_gbr(0);
        usb_ctrl->reg_25 = 15;
        _builtin_set_gbr(0);
        if (usb_ctrl->reg_25 != 15) {
            _builtin_set_gbr(0);
            usb_ctrl->usb_bank = 0;
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_gbr(0);
        usb_ctrl->reg_25 = 0;
        _builtin_set_gbr(0);
        if (usb_ctrl->reg_25 != 0) {
            _builtin_set_gbr(0);
            usb_ctrl->usb_bank = 0;
            _builtin_set_imask(0);
            return 0;
        }
    }
    _builtin_set_gbr(0);
    usb_ctrl->chip_reset.b.b0 = 1;
    _builtin_set_gbr(0);
    usb_ctrl->main_int_enb = 0;
    _builtin_set_gbr(0);
    usb_ctrl->usb_control = 0;
    _builtin_set_gbr(0);
    usb_ctrl->xcvr_control = 0x41;
    _builtin_set_gbr(0);
    usb_ctrl->usb_test = 0;
    _builtin_set_gbr(0);
    usb_ctrl->usb_bank = 1;
    _builtin_set_gbr(0);
    usb_ctrl->reg_20 = 16;
    _builtin_set_gbr(0);
    usb_ctrl->ep0_int_stat.v = usb_ctrl->ep0_int_stat.v;
    _builtin_set_gbr(0);
    usb_ctrl->epa_int_stat = usb_ctrl->epa_int_stat;
    _builtin_set_gbr(0);
    usb_ctrl->epb_int_stat = usb_ctrl->epb_int_stat;
    _builtin_set_gbr(0);
    usb_ctrl->reg_06 = usb_ctrl->reg_06;
    _builtin_set_gbr(0);
    usb_ctrl->reg_02 = usb_ctrl->reg_02;
    _builtin_set_gbr(0);
    usb_ctrl->reg_02 = usb_ctrl->reg_02;
    _builtin_set_gbr(0);
    usb_ctrl->sie_int_stat = usb_ctrl->sie_int_stat;
    _builtin_set_gbr(0);
    usb_ctrl->main_int_stat.v = usb_ctrl->main_int_stat.v;
    _builtin_set_gbr(0);
    usb_ctrl->reg_22 = 6;
    _builtin_set_gbr(0);
    usb_ctrl->reg_23 = 1;
    _builtin_set_gbr(0);
    usb_ctrl->reg_24 = 0;
    _builtin_set_gbr(0);
    usb_ctrl->reg_25 = 0;
    _builtin_set_gbr(0);
    usb_ctrl->usb_bank = 2;
    /* bank 2 +97..+111: FIFO partition 0x200, 0x600, 0xA00, 0xA00 (EPa, EPb 1 KB each) */
    _builtin_set_gbr(0);
    usb_ctrl->reg_30 = 2;
    _builtin_set_gbr(0);
    usb_ctrl->reg_31 = 0;
    _builtin_set_gbr(0);
    usb_ctrl->reg_32 = 6;
    _builtin_set_gbr(0);
    usb_ctrl->reg_33 = 0;
    _builtin_set_gbr(0);
    usb_ctrl->reg_34 = 10;
    _builtin_set_gbr(0);
    usb_ctrl->reg_35 = 0;
    _builtin_set_gbr(0);
    usb_ctrl->reg_36 = 10;
    _builtin_set_gbr(0);
    usb_ctrl->reg_37 = 0;
    _builtin_set_imask(0);
    return 1;
}

void hw_select_hostif(void)
{
    int saved_imask;
    unsigned char hostif;
    unsigned char vbus;
    saved_imask = _builtin_get_imask();
    vbus = usb_read_vbus();
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.selected && hw_hostif_flags.f.usb) {
        if (hw_restart_flags.f.usb_restart) {
            _builtin_set_imask(saved_imask);
            return;
        }
        hw_switch_to_usb();
        _builtin_set_gbr(0);
        usb_ctrl->main_int_enb = 0xc4;
        _builtin_set_gbr(0);
        usb_ctrl->sie_int_enb = 0x86;
        _builtin_set_gbr(0);
        usb_ctrl->reg_0a = 7;
        _builtin_set_gbr(0);
        usb_ctrl->usb_bank = 0;
        _builtin_set_imask(saved_imask);
        return;
    }
    if (vbus == 1 && (settings_byte_0x7f[0] & 0x80)) {
        hostif = 2;
        hw_hostif_flags.f.selected = 1;
        ((struct vdcc_gpio_reg_05 *)p_vdcc_front_gpio[0])->reg_105.b7 = 0;
    } else {
        if (vbus == 1 || hw_restart_flags.v != 0) {
            if (hw_restart_flags.f.usb_restart == 0) {
                if (hw_hostif_flags.f.selected && !hw_hostif_flags.f.usb) {
                    hostif = 1;
                } else {
                    hostif = 2;
                }
                hw_hostif_flags.f.selected = 1;
                goto select;
            }
        }
        hw_hostif_flags.f.selected = 0;
        hostif = 0;
    }
select:
    if ((settings_byte_0x7f[0] & 0x80) == 0 && (hostif == 0 || hostif == 1)) {
        hw_hostif_flags.f.usb = 0;
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode1.b.b5 = 1;
        _builtin_set_gbr(0);
        scsi_ctrl->command = 12;
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b7 = 1;
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b6 = 0;
        _builtin_set_gbr(0);
        while (scsi_ctrl->scsiint1.v != 0) {
            _builtin_set_gbr(0);
            hostif = scsi_ctrl->scsiint1.v;
            _builtin_set_gbr(0);
            scsi_ctrl->scsiint1.v = hostif;
            _builtin_set_gbr(0);
            hostif = scsi_ctrl->scsiint2.v;
            _builtin_set_gbr(0);
            scsi_ctrl->scsiint2.v = hostif;
            _builtin_set_gbr(0);
            hostif = scsi_ctrl->mainint.v;
            _builtin_set_gbr(0);
            scsi_ctrl->mainint.v = hostif;
        }
    }
    if (hostif == 0 || hostif == 2) {
        if (hostif == 2) {
            hw_switch_to_usb();
        }
        _builtin_set_gbr(0);
        usb_ctrl->main_int_enb = 0xc4;
        _builtin_set_gbr(0);
        usb_ctrl->sie_int_enb = 0x86;
        _builtin_set_gbr(0);
        usb_ctrl->reg_0a = 7;
        _builtin_set_gbr(0);
        usb_ctrl->usb_bank = 0;
        if ((usb_wake_events.f.attach_raised) == 0) {
            usb_event_flags.f.attach = 1;
            usb_wake_events.f.attach_raised = 1;
        }
    }
    _builtin_set_imask(saved_imask);
}

int hw_switch_to_usb(void)
{
    char status;
    int switched;
    switched = 0;
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode0.v = 0x43;
    _builtin_set_gbr(0);
    status = scsi_ctrl->scsiint1.v;
    _builtin_set_gbr(0);
    scsi_ctrl->scsiint1.v = status;
    _builtin_set_gbr(0);
    status = scsi_ctrl->scsiint2.v;
    _builtin_set_gbr(0);
    scsi_ctrl->scsiint2.v = status;
    _builtin_set_gbr(0);
    status = scsi_ctrl->mainint.v;
    _builtin_set_gbr(0);
    scsi_ctrl->mainint.v = status;
    _builtin_set_gbr(0);
    scsi_ctrl->config0.b.b7 = 0;
    hw_hostif_flags.f.selected = 1;
    hw_hostif_flags.f.usb = 1;
    ((struct vdcc_gpio_reg_05 *)p_vdcc_front_gpio[0])->reg_105.b7 = 0;
    hw_hostif_flags.f.own_hostif = 1;
    hw_cga_board_present.f.present = 0;
    scsi_initiator_bit.u = 0x80;
    if (((volatile struct board_ctrl_reg_04 *)p_board_ctrl)->reg_04.hostif_idle_enable == 1) {
        switched = 1;
        ((volatile struct board_ctrl_reg_04 *)p_board_ctrl)->reg_04.hostif_idle_enable = 0;
    }
    p_pisces[0]->m.a = 2;
    p_pisces[1]->m.a = 2;
    p_pisces_hostif[0]->ctrl.f.a = 2;
    p_pisces_hostif[0]->ctrl.f.b = 1;
    p_pisces_hostif[1]->ctrl.f.a = 2;
    p_pisces_hostif[1]->ctrl.f.b = 1;
    if (switched) {
        if (power_state.f.leave_request == 0) {
            ((volatile struct board_ctrl_reg_04 *)p_board_ctrl)->reg_04.hostif_idle_enable = 1;
        }
    }
}

#pragma section H
struct vdcc_reg_01 { char pad_00[1]; volatile struct Bits reset_ctrl; };

void selftest_enable_uart_rx(void)
{
    if (maint_mode_word.v & 0x5000) {
        return;
    }
    _builtin_set_imask(15);
    ((struct uart_regs_selftest *)p_uart[0])->ier.b3 = 0;
    ((struct uart_regs_selftest *)p_uart[0])->ier.b2 = 0;
    ((struct uart_regs_selftest *)p_uart[0])->ier.b1 = 0;
    ((struct uart_regs_selftest *)p_uart[0])->ier.b0 = 1;
    ((struct uart_regs_selftest *)p_uart[0])->mcr.b0 = 1;
    ((struct uart_regs_selftest *)p_uart[0])->mcr.b1 = 0;
    _builtin_set_imask(0);
}

void selftest_restart_uart(void)
{
    if (selftest_uart_restart_pending != 0) {
        selftest_uart_restart_pending = 0;
        while (1) {
            if (tick_motor_test_ticks.v > 1000) {
                ((struct vdcc_reg_01 *)p_vdcc_front[0])->reset_ctrl.b3 = 1;
                while (1) {
                    if (!((struct vdcc_reg_01 *)p_vdcc_front[0])->reset_ctrl.b3) {
                        break;
                    }
                }
                hw_init_uart();
                selftest_enable_uart_rx();
                break;
            }
        }
    }
}

void selftest_select_maint_mode(void)
{
    unsigned short z;
    unsigned short display_sub;
    unsigned short display_main;
    unsigned char button_bits;
    unsigned char sensors;

    panel_current.function_number = 1;
    maint_mode_word.v = 0;
    display_sub = 0;
    display_main = 4;
    button_bits = selftest_startup_buttons;
    sensors = selftest_startup_sensors;
    if (sensors & MDC_SENSOR_ADF_COVER_OPEN) {
        if (button_bits & BUTTON_STATE_SCAN) {
            settings_test_flags[0] &= 0xfb;
            maint_screen = MAINT_SCREEN_SELECT;
            display_sub = 1;
            display_main = PANEL_MODE_MAINT;
            /* low byte = maint_mode_word (member c.mode), the maintenance mode number */
            maint_mode_word.v = 0x4000 | MAINT_MODE_FEED_SENSOR_TEST;
        } else {
            if ((button_bits & BUTTON_STATE_FUNCTION) && (button_bits & BUTTON_STATE_SEND_TO)) {
                settings_test_flags[0] &= 0xfb;
                maint_pick_test = 1;
                maint_screen = MAINT_SCREEN_SELECT;
                display_sub = 1;
                display_main = PANEL_MODE_MAINT;
                maint_mode_word.v = 0x4000 | MAINT_MODE_FEED_SENSOR_TEST;
            } else {
                if ((sensors & MDC_SENSOR_NO_PAPER) && (sensors & MDC_SENSOR_TOP)) {
                    maint_save_eeprom_mode.b7 = 1;
                    maint_screen = MAINT_SCREEN_SAVE_EEPROM;
                    display_sub = 7;
                    display_main = PANEL_MODE_MAINT;
                    maint_mode_word.v = 0x4000 | MAINT_MODE_EEPROM_RESTORE;
                } else {
                    if (button_bits & BUTTON_STATE_FUNCTION) {
                        maint_screen = MAINT_SCREEN_SELECT;
                        display_sub = 10;
                        display_main = PANEL_MODE_MAINT;
                        maint_mode_word.v = 0x4000 | MAINT_MODE_EMULATION;
                    }
                }
            }
        }
    } else {
        if ((button_bits & BUTTON_STATE_SCAN) && (button_bits & BUTTON_STATE_SEND_TO)) {
            settings_test_flags[0] &= 0xfb;
            maint_screen = MAINT_SCREEN_SELECT;
            display_sub = 1;
            display_main = PANEL_MODE_MAINT;
            maint_mode_word.v = 0x4000;
        }
    }
    if (maint_mode_word.v & 0x4000) {
        _builtin_set_imask(15);
        hw_hostif_flags.f.cga_test_scan = 0;
        hw_cga_board_present.f.present = 0;
        p_intc->IPRA.WORD &= 0xf0ff;
        if (hw_board_variant == 0) {
            ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.b11 = 1;
        }
        ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.cga_reset = 1;
        ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0c.cga_select = 0;
        _builtin_set_imask(0);
    }
    if (panel_current.display_main == PANEL_MODE_ERROR) {
        display_sub = panel_current.display_sub;
        display_main = PANEL_MODE_ERROR;
    }
    panel_set_display_mode(display_sub, display_main);
    if (maint_mode_word.v & 0x5000) {
        hw_init_scsi_ctrl();
        hw_init_usb_ctrl();
    }
}


void selftest_init_window(void)
{
    unsigned short binarize;
    int x;

    win_params.resolution_mode.v = 1;
    win_params.x_res_dpi.v = img_resolution_codes_2[win_params.resolution_mode.f.resolution_code];
    win_params.y_res_dpi.v = win_params.x_res_dpi.v;
    win_params.x_zoom_percent.v = 100;
    win_params.y_zoom_percent.v = 100;
    win_params.flags.f.adf = 0;
    win_params.paper_size.f.auto_length = 1;
    win_params.paper_size.f.paper_code = 0;
    win_params.paper_size.v |= hw_window_mode_default();
    win_params.side[0].composition.v = 0;
    win_params.side[1].composition.v = 0;
    win_params.side[0].white_level_follow.f.off = 1;
    win_params.side[1].white_level_follow.f.off = 1;
    binarize = 0x400;
    win_params.side[0].binarize.v = binarize;
    win_params.side[1].binarize.v = binarize;
    win_params.side[0].dtc_options = SCSI_WIN_DTC_OPTIONS_DEFAULT;
    win_params.side[1].dtc_options = SCSI_WIN_DTC_OPTIONS_DEFAULT;
    link_window_params_ptrs[0]->dtc_options_hi = 0xa6;
    link_window_params_ptrs[0]->dtc_options_lo = 0x20;
    util_mem_clear(img_mode, 24);
    util_mem_clear(img_mode_applied, 24);
    if (settings_byte_0x49[0] == 0xff) {
        selftest_settings_0x49_value = 0;
    } else if (settings_byte_0x49[0] == 0) {
        selftest_settings_0x49_value = 60;
    } else {
        selftest_settings_0x49_value = settings_byte_0x49[0];
    }
    if (hw_hostif_flags.f.own_hostif || hw_restart_flags.f.unused) {
        win_params.b13.f.b0 = 1;
        win_params.paper_size.f.auto_length = 0;
        if (maint_mode_word.v & 0x5000) {
            win_params.paper_size.f.auto_length = 1;
        } else if (job_mode_params[0].auto_length & 0x80) {
            win_params.paper_size.f.auto_length = 1;
        }
    } else {
        if (maint_mode_word.v & 0x5000) {
            win_params.b13.f.b0 = 1;
        }
        if (hw_hostif_flags.f.cga_test_scan) {
            win_params.b13.f.b0 = 1;
        }
    }
    if (hw_cga_board_type != 0) {
        link_global_params_ptr->scan_flags.scan_mode = 3;
        img_link_scan_mode = 3;
    } else {
        link_global_params_ptr->scan_flags.scan_mode = 2;
        img_link_scan_mode = 2;
    }
    win_sides.v = 0;
    scsi_imprinter_flags.f.b1 = 1;
    if (hw_hostif_flags.f.cga_test_scan) {
        win_params.flags.f.adf = 1;
    }
}

unsigned char hw_window_mode_default(void)
{
    unsigned char mode;

    mode = settings_window_mode[0] & 0x83;
    if (mode & 0x80) {
        mode += 4;
    }
    mode &= 7;
    return hw_window_mode_codes[mode];
}

void selftest_init_tone_curves(void)
{
    char buf[20];

    img_build_tone_curves_front();
    img_build_tone_curves_back();
}

void selftest_run_startup(void)
{
    unsigned char code;

    main_startup_flags.b1 = 1;
    if (selftest_run_power_on() == 0) {
        selftest_show_error(0x89);
    }
    selftest_mark_offset = 0x40;
    code = 0;
    if ((settings_mark_flags[0] & 0x80) && selftest_mdc_in_boot == 0 && err_shipping_lock_pending == 0) {
        code = selftest_measure_mark();
        if (code == 0) {
            selftest_find_mark();
        }
    }
    selftest_select_maint_mode();
}


#define COPY_SAVE() \
    panel_current.saved_main = panel_current.display_main; \
    panel_current.saved_sub = panel_current.display_sub
#define COPY_RESTORE() \
    panel_current.display_main = panel_current.saved_main; \
    panel_current.display_sub = panel_current.saved_sub

void selftest_finish_startup(void)
{
    int i;
    struct page_record *current_page;
    unsigned char saved_flags;

    current_page = &page_records[page_current.v];
    if (hw_device_errors.f.panel_expander) {
        err_pending_flags.f.panel_expander = 1;
        if (!panel_current.startup_error_request.f.b7 && !panel_current.startup_error_shown.f.b7) {
            panel_current.startup_error_request.f.b7 = 1;
            COPY_SAVE();
            panel_set_display_mode(PANEL_ERROR_E6, PANEL_MODE_ERROR); /* E6 operator panel error */
        }
        while (!panel_current.startup_error_shown.f.b7) {
        }
        COPY_RESTORE();
    }
    if (hw_device_errors.f.eeprom) {
        err_pending_flags.f.eeprom = 1;
        if (!panel_current.startup_error_request.f.b6 && !panel_current.startup_error_shown.f.b6) {
            panel_current.startup_error_request.f.b6 = 1;
            COPY_SAVE();
            panel_set_display_mode(PANEL_ERROR_E7, PANEL_MODE_ERROR); /* E7 EEPROM error */
        }
        while (!panel_current.startup_error_shown.f.b6) {
        }
        COPY_RESTORE();
    }
    if (settings_byte_0x7f[0] & 0x80) {
        hw_device_errors.f.scsi_interface = 0;
    }
    if (hw_device_errors.f.scsi_interface) {
        err_pending_flags.f.scsi_interface = 1;
        if (!panel_current.startup_error_request.f.b5 && !panel_current.startup_error_shown.f.b5) {
            panel_current.startup_error_request.f.b5 = 1;
            COPY_SAVE();
            panel_set_display_mode(PANEL_ERROR_E8, PANEL_MODE_ERROR); /* E8 SCSI interface circuit error */
        }
        while (!panel_current.startup_error_shown.f.b5) {
        }
        COPY_RESTORE();
    }
    if (hw_device_errors.f.image_memory) {
        err_pending_flags.f.image_memory = 1;
        if (!panel_current.startup_error_request.f.b4 && !panel_current.startup_error_shown.f.b4) {
            panel_current.startup_error_request.f.b4 = 1;
            COPY_SAVE();
            panel_set_display_mode(PANEL_ERROR_E9, PANEL_MODE_ERROR); /* E9 image memory error */
        }
        while (!panel_current.startup_error_shown.f.b4) {
        }
        COPY_RESTORE();
    }
    if (selftest_mdc_in_boot != 0) {
        if (!panel_current.startup_error_request.f.b3 && !panel_current.startup_error_shown.f.b3) {
            panel_current.startup_error_request.f.b3 = 1;
            COPY_SAVE();
            if (iic_panel_set_digit(panel_glyph_f_pattern[0])) {
                panel_current.glyph_shown = panel_glyph_f_pattern[0];
            }
        }
        while (1) {
            _builtin_set_imask(0);
            if (flash_download_session & 0x80) {
                break;
            }
        }
        COPY_RESTORE();
    }
    if (err_pending_flags.f.cga_board && !hw_hostif_flags.f.own_hostif && !hw_restart_flags.f.leaving_power_save && !hw_restart_flags.v) {
        COPY_SAVE();
        panel_set_display_mode(PANEL_ERROR_H5, PANEL_MODE_ERROR); /* H5 CGA board error: go on with the scanner's own SCSI/USB */
        hw_cga_board_present.f.present = 0;
        hw_hostif_flags.f.cga_test_scan = 0;
        hw_hostif_flags.f.own_hostif = 1;
        win_params.side[0].size_rounding.v = 0x6002;
        ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.b7 = 0;
        hw_init_scsi_ctrl();
        hw_init_usb_ctrl();
        hw_select_hostif();
    }
    if (panel_current.display_main == PANEL_MODE_ERROR) {
        panel_set_display_mode(panel_current.display_sub, PANEL_MODE_ERROR);
    } else {
        if (maint_mode_word.v & 0x5000) {
            _builtin_set_imask(15);
            selftest_startup_done = 1;
            _builtin_set_imask(0);
            panel_set_display_mode(1, PANEL_MODE_MAINT);
            win_params.flags.f.adf = 1;
            afe_select_source();
            while (button_state.b.b7) {
            }
        } else {
            panel_set_display_mode(0, PANEL_MODE_FUNCTION);
        }
    }
    if (!hw_restart_flags.f.leaving_power_save && !hw_restart_flags.f.host_reset && !hw_restart_flags.f.usb_restart) {
        win_params.flags.f.adf = 1;
        afe_select_source();
        mdc_run_end_heater_warmup(23);
        _builtin_set_imask(15);
        tick_lamp_on_ticks.v = 0;
        _builtin_set_imask(0);
        power_restart_sensor_path(0);
    }
    if (!hw_hostif_flags.f.selected && hw_cga_board_present.f.present) {
        if (page_records[0].error_flags.v == 0) {
            selftest_restart_uart();
            _builtin_set_imask(15);
            tick_wait_count = 0;
            _builtin_set_imask(0);
            while (tick_wait_count < 15000) {
                if (hw_hostif_flags.f.owner_decided || hw_cga_board_present.f.link_identified) {
                    break;
                }
                if (maint_mode_word.v & 0x5000) {
                    break;
                }
            }
        }
    }
    _builtin_set_imask(15);
    mdc_fan_alarm.b7 = 1;
    selftest_startup_done = 1;
    tick_hopper_was_empty = 1;
    if (hw_restart_flags.f.host_reset || hw_restart_flags.f.usb_restart) {
        hw_restart_flags.f.host_reset = 0;
        hw_restart_flags.f.usb_restart = 0;
        _builtin_set_imask(0);
        if (settings_byte_0x7f[0] & 1) {
            saved_flags = win_params.flags.v;
            win_params.flags.f.adf = 0;
            afe_select_source();
            win_params.flags.v = saved_flags;
        }
        if (hw_hostif_flags.f.own_hostif) {
            return;
        }
        _builtin_set_imask(15);
        if (current_page->error_flags.c.f.hardware_error) {
            link_reply.c.code = 6;
        } else {
            link_reply.c.code = 1;
        }
        link_reply.f.b7 = 1;
        isr_request_command_layer(2);
        _builtin_set_imask(0);
        if (hw_restart_flags.f.unused) {
            for (i = 0; i < 200; i++) {
                rt_delay_60k();
            }
            hw_hostif_flags.f.own_hostif = 1;
            ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.b7 = 0;
            ((struct uart_regs_selftest *)p_uart[0])->ier.b3 = 0;
            ((struct uart_regs_selftest *)p_uart[0])->ier.b2 = 0;
            ((struct uart_regs_selftest *)p_uart[0])->ier.b1 = 0;
            ((struct uart_regs_selftest *)p_uart[0])->ier.b0 = 0;
            ((struct uart_regs_selftest *)p_uart[0])->mcr.b0 = 0;
            ((struct uart_regs_selftest *)p_uart[0])->mcr.b1 = 0;
        }
        _builtin_set_imask(15);
        hw_restart_flags.v = 0;
        _builtin_set_imask(0);
        return;
    }
    if ((mdc_mailbox.df_adjust_flags & (MDC_DF_ADJUST_ULTRASONIC_OK | MDC_DF_ADJUST_DF_OK)) != (MDC_DF_ADJUST_ULTRASONIC_OK | MDC_DF_ADJUST_DF_OK)) {
        win_params.flags.f.adf = 1;
        _builtin_set_imask(0);
        afe_select_source();
    } else {
        if ((settings_byte_0x7f[0] & 1) && !hw_restart_flags.f.leaving_power_save && page_records[0].error_flags.v == 0 && !(maint_mode_word.v & 0x5000)) {
            tick_wait_count = 0;
            _builtin_set_imask(0);
            while (tick_wait_count < 10000) {
                if (tick_heater_monitor == 0) {
                    saved_flags = win_params.flags.v;
                    win_params.flags.f.adf = 0;
                    afe_select_source();
                    win_params.flags.v = saved_flags;
                    break;
                }
                if (hw_hostif_flags.f.own_hostif) {
                    if (page_records[0].flags.f.load_pending || page_records[0].sides[0].flags.f.requested || (job_manual_feed_flags.w & 0x8000)) {
                        break;
                    }
                    if (job_aborted) {
                        break;
                    }
                    continue;
                }
            }
        }
    }
    _builtin_set_imask(0);
    return;
}

struct vdcc_reg_0b_2 {
    char p[11];
    struct { unsigned char :5, gamma_enable:3; } output_ctrl;
};

int selftest_vdcc_small_rams(void)
{
    short pattern;

    ((struct vdcc_reg_0b_2 *)((struct vdcc_reg_01 *)p_vdcc_front[0]))->output_ctrl.gamma_enable = 0;
    ((struct vdcc_reg_0b_2 *)p_vdcc_back[0])->output_ctrl.gamma_enable = 0;
    p_vdcc_front_bank[0]->test_pattern = 0;
    p_vdcc_back_bank[0]->test_pattern = 0;
    p_vdcc_front_bank[0]->bank = 0;
    p_vdcc_back_bank[0]->bank = 0;
    rt_delay_2400();
    rt_delay_2400();
    pattern = 0xff;
    while (1) {
        if (hw_fill_verify_16(p_vdcc_front_gamma_0, 0x400, pattern) == 0) {
            return 0x80;
        }
        if (hw_fill_verify_16(p_vdcc_front_gamma_1, 0x400, pattern) == 0) {
            return 0x40;
        }
        if (hw_fill_verify_16(p_vdcc_front_gamma_2, 0x400, pattern) == 0) {
            return 0x20;
        }
        if (hw_fill_verify_16(p_vdcc_back_gamma_0, 0x400, pattern) == 0) {
            return 0x8080;
        }
        if (hw_fill_verify_16(p_vdcc_back_gamma_1, 0x400, pattern) == 0) {
            return 0x8040;
        }
        if (hw_fill_verify_16(p_vdcc_back_gamma_2, 0x400, pattern) == 0) {
            return 0x8020;
        }
        if (pattern == 0) {
            break;
        }
        pattern -= 85;
    }
    pattern = 0x3ff;
    while (1) {
        if (selftest_fill_verify(p_vdcc_front_test_table, 0x100, pattern)) {
            return 16;
        }
        if (selftest_fill_verify(p_vdcc_back_test_table, 0x100, pattern)) {
            return 0x8010;
        }
        if (pattern == 0) {
            break;
        }
        pattern -= 341;
    }
    ((struct vdcc_reg_0b_2 *)((struct vdcc_reg_01 *)p_vdcc_front[0]))->output_ctrl.gamma_enable = 7;
    ((struct vdcc_reg_0b_2 *)p_vdcc_back[0])->output_ctrl.gamma_enable = 7;
    return 0;
}


unsigned short selftest_vdcc_bank_rams()
{
    short pattern;
    unsigned short result = 0;
    unsigned short saved_front_modes[4];
    unsigned short saved_back_modes[4];

    saved_front_modes[0] = p_vdcc_front_correction[0]->mode_chip0;
    saved_front_modes[1] = p_vdcc_front_correction[0]->mode_chip1;
    saved_front_modes[2] = p_vdcc_front_correction[0]->mode_chip2;
    saved_back_modes[0] = p_vdcc_back_correction[0]->mode_chip0;
    saved_back_modes[1] = p_vdcc_back_correction[0]->mode_chip1;
    saved_back_modes[2] = p_vdcc_back_correction[0]->mode_chip2;
    p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_RAW;
    p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_RAW;
    p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_RAW;
    p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_RAW;
    p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_RAW;
    p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_RAW;
    p_vdcc_front_bank[0]->bank = 1;
    p_vdcc_back_bank[0]->bank = 1;
    rt_delay_2400();
    rt_delay_2400();
    pattern = 0x3ff;
    while (1) {
        if (selftest_fill_verify(p_vdcc_front_ref_0[0], 0x2000, pattern)) {
            result = 8;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_front_ref_byte_0[0], 0x2000, (unsigned char)pattern)) {
            result = 1;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_front_ref_1[0], 0x2000, pattern)) {
            result = 4;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_front_ref_byte_1[0], 0x2000, (unsigned char)pattern)) {
            result = 0x180;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_back_ref_0[0], 0x2000, pattern)) {
            result = 0x8008;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_back_ref_byte_0[0], 0x2000, (unsigned char)pattern)) {
            result = 0x8001;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_back_ref_1[0], 0x2000, pattern)) {
            result = 0x8004;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_back_ref_byte_1[0], 0x2000, (unsigned char)pattern)) {
            result = 0x8180;
            goto end;
        }
        if (pattern == 0) {
            break;
        }
        pattern -= 0x155;
    }
    p_vdcc_front_bank[0]->bank = 2;
    p_vdcc_back_bank[0]->bank = 2;
    rt_delay_2400();
    rt_delay_2400();
    pattern = 0x3ff;
    while (1) {
        if (selftest_fill_verify(p_vdcc_front_ref_2[0], 0x2000, pattern)) {
            result = 2;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_front_ref_byte_2[0], 0x2000, (unsigned char)pattern)) {
            result = 0x140;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_back_ref_2[0], 0x2000, pattern)) {
            result = 0x8002;
            goto end;
        }
        if (selftest_fill_verify(p_vdcc_back_ref_byte_2[0], 0x2000, (unsigned char)pattern)) {
            result = 0x8140;
            goto end;
        }
        if (pattern == 0) {
            break;
        }
        pattern -= 0x155;
    }
end:
    p_vdcc_front_correction[0]->mode_chip0 = saved_front_modes[0];
    p_vdcc_front_correction[0]->mode_chip1 = saved_front_modes[1];
    p_vdcc_front_correction[0]->mode_chip2 = saved_front_modes[2];
    p_vdcc_back_correction[0]->mode_chip0 = saved_back_modes[0];
    p_vdcc_back_correction[0]->mode_chip1 = saved_back_modes[1];
    p_vdcc_back_correction[0]->mode_chip2 = saved_back_modes[2];
    return result;
}


unsigned short selftest_vdcc_bank_select()
{
    unsigned short result;
    short pattern;
    unsigned short bank;
    int back;

    p_vdcc_capture[0]->x_res_ctrl.b.b7 = 0;
    p_vdcc_capture[1]->x_res_ctrl.b.b7 = 0;
    for (bank = 3; bank < 7; bank++) {
        p_vdcc_front_bank[0]->bank = bank;
        p_vdcc_back_bank[0]->bank = bank;
        rt_delay_2400();
        rt_delay_2400();
        pattern = 0xff;
        while (1) {
            back = 0;
            if (selftest_fill_verify(p_vdcc_front_ref_0[0], 0x2000, pattern)) {
                goto found;
            }
            back = 1;
            if (selftest_fill_verify(p_vdcc_back_ref_0[0], 0x2000, pattern)) {
                goto found;
            }
            if (pattern == 0) {
                break;
            }
            pattern -= 85;
        }
    }
    p_vdcc_front_bank[0]->bank = 0;
    p_vdcc_back_bank[0]->bank = 0;
    return 0;
found:
    switch (bank) {
    case 3:
        result = 0x120;
        break;
    case 4:
        result = 0x110;
        break;
    default:
        result = 0x108;
        break;
    }
    if (back) {
        result |= 0x8000;
    }
    return result;
}
#pragma section


short selftest_pisces_indirect(void)
{
    int side;
    unsigned short result;
    for (side = 0; side <= 1; side++) {
        result = selftest_pisces_indirect_ram(side);
        if (result != 0) {
            if (side != 0) {
                result |= 0x8000;
            }
            break;
        }
    }
    return result;
}

short selftest_pisces_indirect_ram(int side)
{
    short result;
    unsigned short pattern;
    unsigned short page;
    short *sdram;
    p_pisces_mode[side]->run.v = 0;
    p_pisces_ctrl[side]->ctrl.f.sdram_window = 1;
    if (side == 0) {
        sdram = p_pisces_front_sdram.h;
    } else {
        sdram = p_pisces_back_sdram.h;
    }
    result = 0x104;
    for (page = 0; page < 0x1800; page += 5) {
        p_pisces_ctrl[side]->reg_006 = page;
        p_pisces_ctrl[side]->sdram_page = page;
        for (pattern = 0xffff; ; pattern -= 0x5555) {
            if (selftest_fill_verify_stride(sdram, pattern) != 0) {
                goto done;
            }
            if (pattern == 0) {
                break;
            }
        }
    }
    result = 0;
done:
    p_pisces_ctrl[side]->ctrl.f.sdram_window = 0;
    if (result == 0) {
        selftest_memory_error_code.v = 0;
    } else if (side == 0) {
        if (page > 0x7ff) {
            selftest_memory_error_code.v = ERR_IMAGE_MEMORY_EXT_FRONT;
        } else {
            selftest_memory_error_code.v = ERR_IMAGE_MEMORY_FRONT;
        }
    } else if (page > 0x7ff) {
        selftest_memory_error_code.v = ERR_IMAGE_MEMORY_EXT_BACK;
    } else {
        selftest_memory_error_code.v = ERR_IMAGE_MEMORY_BACK;
    }
    return result;
}

int selftest_pisces_rams(void)
{
    short result;
    short pattern;
    int back;
    unsigned int failed_at;
    p_pisces_mode[0]->run.v = 0;
    p_pisces_mode[1]->run.v = 0;
    pattern = 0xff;
    while (1) {
        back = 0;
        failed_at = selftest_fill_verify(p_pisces_front_ram, 0x1500, pattern);
        if (failed_at != 0) {
            goto fail;
        }
        back = 1;
        failed_at = selftest_fill_verify(p_pisces_back_ram, 0x1500, pattern);
        if (failed_at != 0) {
            goto fail;
        }
        if (pattern == 0) {
            break;
        }
        pattern -= 0x55;
    }
    return 0;
fail:
    if (failed_at < 0x1000) {
        result = 0x102;
    } else if (failed_at < 0x1100) {
        result = 0x280;
    } else if (failed_at < 0x1200) {
        result = 0x240;
    } else if (failed_at < 0x1300) {
        result = 0x220;
    } else if (failed_at < 0x1400) {
        result = 0x210;
    } else {
        result = 0x210;
    }
    if (back) {
        result |= 0x8000;
    }
    return result;
}

unsigned int selftest_fill_verify(short *area, unsigned int count, short value)
{
    unsigned int i;
    short *q;
    q = area;
    for (i = 0; i < count; i++) {
        *q++ = value;
    }
    q = area;
    i = 0;
    while (i < count) {
        if (*q != value) {
            if (i == 0) {
                i++;
            }
            return i;
        }
        q++;
        i++;
    }
    return 0;
}

unsigned int selftest_fill_verify_stride(short *area, short value)
{
    unsigned int i;
    unsigned int count;
    int step;
    short *q;
    q = area;
    if (testmode_number.v == 0) {
        count = 0x31;
        step = 0x555;
    } else {
        count = 0x304;
        step = 0x55;
    }
    for (i = 0; i < count; i++) {
        *q = value;
        q += step;
    }
    q = area;
    i = 0;
    while (i < count) {
        if (*q != value) {
            if (i == 0) {
                i++;
            }
            return i;
        }
        q += step;
        i++;
    }
    return 0;
}

#pragma section H
char selftest_map_error(unsigned short result)
{
    unsigned char code = 0;
    unsigned short low_bits;

    if (result & 0x8000) {
        low_bits = result & 0x7fff;
        switch (low_bits & 0xff00) {
        case 0:
            code = 0xe5;
            break;
        case 0x100:
            if (low_bits == 0x104) {
                code = 0xe4;
            } else if (low_bits == 0x102) {
                code = 0xe6;
            } else {
                code = 0xe5;
            }
            break;
        default:
            code = 0xe6;
            break;
        }
    } else {
        switch (result & 0xff00) {
        case 0:
            code = 0xe5;
            break;
        case 0x100:
            if (result == 0x104) {
                code = 0xe4;
            } else if (result == 0x102) {
                code = 0xe6;
            } else {
                code = 0xe5;
            }
            break;
        default:
            code = 0xe6;
            break;
        }
    }
    return code;
}

struct vdcc_monitor_selftest {
    unsigned char reg_060;
    unsigned char x:2,
                  mode:2,
                  y:4;
    char reg_062_067[6];
    unsigned short position_left;
};
struct vdcc_counter_reg_0b_2 {
    char p[11];
    struct vdcc_counter_ctrl_bits counter_ctrl;
};


unsigned char selftest_measure_mark(void)
{
    mdc_run_lamps(0x99);
    win_params.flags.f.adf = 0;
    afe_select_source();
    afe_set_gain(0, 1, 0, 0x3fe);
    afe_set_gain(0, 1, 1, 0x3fe);
    _builtin_set_imask(15);
    ((volatile struct board_ctrl_regs_selftest *)p_board_ctrl)->reg_0e.ccd_300dpi_mode = 0;
    p_vdcc_front_sensor->source_gate.b.b2 = 0;
    p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    _builtin_set_imask(0);
    ((struct vdcc_monitor_selftest *)p_vdcc_front_monitor[0])->mode = 2;
    ((struct vdcc_monitor_selftest *)p_vdcc_front_monitor[0])->y = 7;
    ((struct vdcc_monitor_selftest *)p_vdcc_front_monitor[0])->position_left = 0x396;
    ((struct vdcc_counter_reg_0b_2 *)p_vdcc_back_counter[0])->counter_ctrl.back_run = 0;
    ((struct vdcc_counter_reg_0b_2 *)p_vdcc_back_counter[0])->counter_ctrl.back_clear = 1;
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    while (((struct vdcc_counter_reg_0b_2 *)p_vdcc_back_counter[0])->counter_ctrl.back_clear) {
        if (tick_wait_count >= 5000) {
            ((struct vdcc_monitor_selftest *)p_vdcc_front_monitor[0])->mode = 0;
            return 0xe9;
        }
    }
    ((struct vdcc_counter_reg_0b_2 *)p_vdcc_back_counter[0])->counter_ctrl.back_run = 1;
    selftest_mark_start = 0;
    selftest_profile_count = 0;
    line_side_state[0].state_flags.f.mark_sampling = 1;
    mdc_run_command(MDC_CMD_FB_MOTOR_HOME);
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    while (line_side_state[0].state_flags.f.mark_sampling) {
        if (tick_wait_count >= 5000) {
            break;
        }
    }
    ((struct vdcc_monitor_selftest *)p_vdcc_front_monitor[0])->mode = 0;
    ((struct vdcc_counter_reg_0b_2 *)p_vdcc_back_counter[0])->counter_ctrl.back_run = 0;
    if (line_side_state[0].state_flags.f.mark_sampling) {
        line_side_state[0].state_flags.f.mark_sampling = 0;
        return 0xf0;
    }
    return 0;
}
#pragma section
