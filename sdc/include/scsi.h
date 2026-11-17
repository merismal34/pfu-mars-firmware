/* scsi.h: declarations of scsi.c, scsi_win_descriptor.c, scsi_win_header.c, scsi_win_pixels.c,
   scsi_win_units.c, scsi_cmd_layer.c and 1 more files. */
#ifndef SCSI_H
#define SCSI_H
#include "common.h"

/* scsi_status: the index into scsi_status_table (scsi_run_status_steps) a handler leaves for the status phase */
#define SCSI_STEP_GOOD                  2
#define SCSI_STEP_CHECK_CONDITION       3
#define SCSI_STEP_RESERVATION_CONFLICT  4
#define SCSI_STEP_BUSY                  5
#define SCSI_STEP_CHECK_CONDITION_FIELD 6
#define SCSI_STEP_NO_STATUS             7
#define SCSI_STEP_SEND_STATUS           8
/* scsi_diag_request: the vendor request SEND DIAGNOSTIC posted for READ DIAGNOSTIC (names from the vendor tool's request strings) */
#define SCSI_DIAG_REQUEST_DEBUG_E2P_WRT    1
#define SCSI_DIAG_REQUEST_DEBUG_E2T_WRT    2
#define SCSI_DIAG_REQUEST_DEBUG_E2T_RED    3
#define SCSI_DIAG_REQUEST_DEBUG_RAM_WRT    4
#define SCSI_DIAG_REQUEST_DEBUG_RAM_RED    5
#define SCSI_DIAG_REQUEST_ADF_SENSOR       7
#define SCSI_DIAG_REQUEST_READ_COUNT       8
#define SCSI_DIAG_REQUEST_FIRST_READ_DATE  11
#define SCSI_DIAG_REQUEST_ROM_INFORMATION  12
#define SCSI_DIAG_REQUEST_READ_COUNT2      13
#define SCSI_DIAG_REQUEST_SDC_BLOCK_TABLE  16
#define SCSI_DIAG_REQUEST_GET_FIRST_DATE   17
#define SCSI_DIAG_REQUEST_LOAD_CANCEL      18
#define SCSI_DIAG_REQUEST_GET_DEVICE_ID    19
#define SCSI_DIAG_REQUEST_GET_EEPROM_STS   21
#define SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_2  24
#define SCSI_DIAG_REQUEST_DEBUG_E2T_RED_2  25
#define SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_W  26
#define SCSI_DIAG_REQUEST_DEBUG_E2T_RED_W  27
#define SCSI_DIAG_REQUEST_MDC_BLOCK_READ   30
#define SCSI_DIAG_REQUEST_TEST_LENGTH      40
#define SCSI_DIAG_REQUEST_TESTMODE         41
#define SCSI_DIAG_REQUEST_SCSI_ID_GET      42
#define SCSI_DIAG_REQUEST_DEFECT_LISTS     43
#define SCSI_DIAG_REQUEST_ERASE_FLASH_WORK 44
#define SCSI_DIAG_REQUEST_GET_READTIME     45
#define SCSI_DIAG_REQUEST_SETUP_START_T01  48
#define SCSI_DIAG_REQUEST_GET_FIRMVERSION  49
#define SCSI_DIAG_REQUEST_SETUP_IMPRINTERR 50
#define SCSI_DIAG_REQUEST_GET_SOP_DEFAULT  51
#define SCSI_DIAG_REQUEST_GET_POWERSWMODE  52
#define SCSI_DIAG_REQUEST_GET_SOP_SET_LEN  55
#define SCSI_DIAG_REQUEST_GET_SOP_SET_DAT  56
#define SCSI_DIAG_REQUEST_PUT_SOP_SET_DAT  57
#define SCSI_DIAG_REQUEST_GET_DEV_LOG_LEN  64
#define SCSI_DIAG_REQUEST_GET_DEV_LOG_DAT  65
#define SCSI_DIAG_REQUEST_DEBUG_E2P_WRT_X  66
#define SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_X  67
#define SCSI_DIAG_REQUEST_DEBUG_E2T_RED_X  68
#define SCSI_DIAG_REQUEST_SET_POWOFF_TIME  84
#define SCSI_DIAG_REQUEST_GET_POWOFF_TIME  85
/* the DTC options default of a window side (win_side.dtc_options) */
#define SCSI_WIN_DTC_OPTIONS_DEFAULT 0xA620

struct scsi_win_paper_size_entry {
    unsigned short paper_code, width_inch1200, length_inch1200;
};
/* the handler: a function of either return type (int or void), called without a result */
struct scsi_cmd { unsigned char opcode; void (*handler)(void); };

/* functions */
extern void scsi_run_layer(void);
extern int scsi_run_layer_step_1(void);
extern void scsi_resume(void);
extern int scsi_handle_command(void);
extern void scsi_read_initiator(void);
extern int scsi_id_to_bit(char);
extern char scsi_handle_command_step_2(void);
extern int scsi_disable_irq(void);
extern int scsi_enable_irq(void);
extern void scsi_wait_ctrl_command(void);
extern void scsi_ctrl_command(char);
extern void scsi_read_interrupts(void);
extern void scsi_handle_bus_reset(void);
extern void scsi_receive_cdb(int count, unsigned char *p);
extern void scsi_send_status_byte(void);
extern void scsi_send_message_byte(void);
extern int scsi_receive_message_byte(int);
extern int scsi_receive_params(void);
extern void scsi_receive_params_step_1();
extern int scsi_send_reply(void);
extern void scsi_send_data_in();
extern void scsi_reselect(void);
extern int scsi_disconnect(void);
extern void scsi_release_bus(void);
extern void scsi_set_sync_mode(void);
extern void scsi_defer_enable_irq(void);
extern void scsi_defer_disable_irq(void);
extern int scsi_check_irq_idle(void);
extern void scsi_wait_dma_in_done(void);
extern void scsi_finish_resume(void);
extern void scsi_run_status_steps(void);
extern int scsi_status_step_0(void);
extern unsigned char scsi_status_step_2(void);
extern unsigned char scsi_status_step_3(void);
extern char scsi_status_step_8(void);
extern char scsi_dispatch(void);
extern void scsi_execute_command();
extern int scsi_cmd_test_unit_ready(void);
extern int scsi_cmd_release_unit(void);
extern int scsi_cmd_reserve_unit(void);
extern int scsi_cmd_inquiry(void);
extern void scsi_cmd_request_sense(void);
extern int scsi_check_ready(void);
extern int scsi_check_ready_2(void);
extern int scsi_lowest_bit();
extern int scsi_initiator_allowed(void);
extern int scsi_check_parity_error(void);
extern int scsi_check_reservation(void);
extern int scsi_check_unit_attention(void);
extern int scsi_check_not_ready(void);
extern int scsi_sense_invalid_cdb(void);
extern void scsi_clear_sync_agreement(void);
extern int scsi_store_sync_agreement(void);
extern void scsi_mark_invalid_field();
extern int scsi_read_check_window_tables();
extern int scsi_inquiry_standard(void);
extern int scsi_inquiry_vpd(void);
extern int scsi_cmd_endorser(void);
extern unsigned short scsi_imprinter_next_slot(void);
extern int scsi_cmd_read(void);
extern int scsi_read_reply(void);
extern int scsi_clear_read_state(void);
extern int scsi_read_start(void);
extern unsigned char scsi_read_check_windows(int request_side, int side_end);
extern void scsi_read_check_windows_step_3(int side);
extern void scsi_run_xfer_front(void);
extern void scsi_run_xfer_back(void);

/* functions */
extern int scsi_finish_command(void);
extern int scsi_run_messages(int);
extern int scsi_message_check_atn(void);
extern int scsi_message_receive(void);
extern int scsi_message_read_fifo(void);
extern int scsi_message_classify(void);
extern int scsi_message_identify(void);
extern int scsi_message_dispatch(void);
extern int scsi_message_extended(void);
extern int scsi_message_device_reset(void);
extern int scsi_message_abort(void);
extern int scsi_message_initiator_error(void);
extern int scsi_message_reject_received(void);
extern int scsi_message_nop(void);
extern int scsi_message_parity_error(void);
extern int scsi_message_send_reject(void);
extern int scsi_message_retry(void);
extern int scsi_message_error(void);
extern int scsi_message_abort_command(void);
extern int scsi_message_next(void);
extern int scsi_message_check_atn_end(void);
extern int scsi_message_send_sdtr(void);
extern int scsi_message_sdtr_answer(void);
extern int scsi_status_step_4(void);
extern int scsi_status_step_5(void);
extern int scsi_status_step_6(void);
extern int scsi_status_step_7(void);
extern void scsi_cmd_send(void);
extern void scsi_cmd_send_step_3(void);
extern void scsi_cmd_set_window(void);
extern void scsi_cmd_object_position(void);
extern void scsi_cmd_send_diagnostic(void);
extern void scsi_cmd_read_diagnostic(void);
extern signed char scsi_sop_item_mask(unsigned short offset);
extern void scsi_build_sop_items(void);
extern void scsi_copy_mdc_log(unsigned short selector, unsigned short offset, unsigned short len);
extern void scsi_copy_device_log(unsigned short offset, unsigned short len);
extern void scsi_cmd_scan(void);
extern void scsi_cmd_mode_sense(void);
extern void scsi_cmd_mode_select(void);
extern void scsi_cmd_get_hw_status(void);
extern void scsi_cmd_scanner_control(void);
extern void scsi_set_sense();
extern void scsi_sense_invalid_param(unsigned char, short);
extern void scsi_init_sense(int clear_key_specific);
extern void scsi_set_good(void);
extern int scsi_reselect_pending(void);
extern int scsi_check_reselection(void);
extern void scsi_win_save_descriptor(void);
extern unsigned int scsi_win_parse_descriptor(unsigned int p);
extern int scsi_win_check_resolution(unsigned short dpi, unsigned short front_dpi, unsigned char side);
extern int scsi_win_check_centered_area(unsigned int paper_width, int side);
extern int scsi_win_check_max_length(unsigned int length);
extern unsigned int scsi_win_check_header(unsigned char *);
extern void scsi_win_inch1200_to_px16(int length, unsigned short dpi);
extern void scsi_win_units_to_inch1200(unsigned int value, unsigned short dpi);
extern void scsi_run_cmd_state(void);
extern int scsi_cmd_state_0(void);
extern int scsi_cmd_state_2(void);
extern int scsi_cmd_state_3(void);
extern int scsi_cmd_state_4(void);
extern int scsi_cmd_state_5(void);
extern int scsi_cmd_state_6(void);
extern void scsi_cmd_set_subwindow(void);
extern void scsi_cmd_get_data_buffer_status(void);
extern void scsi_cmd_get_window(void);
extern void scsi_unused_set_ctl_bit2(void);
extern void scsi_unused_hook(void);
extern void scsi_run_end_state(void);
extern int scsi_end_state_0(void);
extern int scsi_end_state_2(void);
extern int scsi_end_state_3(void);
extern int scsi_end_state_4(void);
extern int scsi_end_state_5(void);
extern int scsi_end_state_6(void);
extern int scsi_end_state_7(void);
extern int scsi_cmd_read_step_7(void);
extern int scsi_cmd_read_step_8(void);

/* constants */
extern const struct scsi_cmd scsi_cmd_table[21];
extern const struct scsi_win_paper_size_entry scsi_win_paper_sizes[13];
extern const unsigned char scsi_sop_item_masks[86];
extern unsigned char (*scsi_status_table[10]) (void);
extern unsigned char (*scsi_end_state_table[8]) (void);
extern unsigned char (*scsi_cmd_state_table[7]) (void);
extern unsigned char (*xfer_state_table[13]) (void);
extern int (*scsi_message_state_table[22]) ();

/* constants */
extern const unsigned char scsi_inquiry_data[118];
extern const unsigned char scsi_inquiry_invalid_lun[118];
extern const unsigned char scsi_inquiry_vpd_data[144];
extern const unsigned char scsi_inquiry_vpd_invalid_lun[52];
extern const char scsi_sense_lun_not_supported[40];
extern const char scsi_sense_reserved[40];
extern const unsigned char scsi_counter_format_5[8];
extern const unsigned char scsi_counter_format_8[8];
extern const unsigned char scsi_keyword_debug_e2p_wrt[20];
extern const unsigned char scsi_keyword_debug_e2t_wrt_w[20];
extern const unsigned char scsi_keyword_debug_e2t_red_w[20];
extern const unsigned char scsi_keyword_setup_start_t01[20];
extern const unsigned char scsi_keyword_get_imp_life[16];
extern const unsigned char scsi_keyword_clear_ink_life[16];
extern const unsigned char scsi_keyword_debug_e2t_wrt[20];
extern const unsigned char scsi_keyword_debug_e2t_red[20];
extern const unsigned char scsi_keyword_debug_e2t_wrt_2[20];
extern const unsigned char scsi_keyword_debug_e2t_red_2[20];
extern const unsigned char scsi_keyword_debug_ram_wrt[20];
extern const unsigned char scsi_keyword_debug_ram_red[20];
extern const unsigned char scsi_keyword_adf_sensor[20];
extern const unsigned char scsi_keyword_read_count[20];
extern const unsigned char scsi_keyword_read_count2[20];
extern const unsigned char scsi_keyword_read_count_clr[16];
extern const unsigned char scsi_keyword_first_read_date[20];
extern const unsigned char scsi_keyword_get_first_date[20];
extern const unsigned char scsi_keyword_get_device_id[20];
extern const unsigned char scsi_keyword_rom_information[20];
extern const unsigned char scsi_keyword_agc_clear[20];
extern const unsigned char scsi_keyword_get_eeprom_sts[20];
extern const unsigned char scsi_keyword_check_prnu_pos[20];
extern const unsigned char scsi_keyword_erase_flash_work[20];
extern const unsigned char scsi_keyword_test_length[20];
extern const unsigned char scsi_keyword_testmode[20];
extern const unsigned char scsi_keyword_scsi_id_get[20];
extern const unsigned char scsi_keyword_load_cancel[20];
extern const unsigned char scsi_keyword_omr_sensor_test[20];
extern const unsigned char scsi_keyword_setup_start[20];
extern const unsigned char scsi_keyword_setup_end[20];
extern const unsigned char scsi_keyword_measure_readtime[20];
extern const unsigned char scsi_keyword_get_readtime[20];
extern const unsigned char scsi_keyword_get_firmversion[20];
extern const unsigned char scsi_keyword_setup_imprinter[20];
extern const unsigned char scsi_keyword_setup_imprinterr[20];
extern const unsigned char scsi_keyword_diag_start[20];
extern const unsigned char scsi_keyword_diag_end[20];
extern const unsigned char scsi_keyword_long_page[20];
extern const unsigned char scsi_keyword_get_sop_default[20];
extern const unsigned char scsi_keyword_get_powerswmode[20];
extern const unsigned char scsi_keyword_get_sop_set_len[20];
extern const unsigned char scsi_keyword_get_sop_set_dat[20];
extern const unsigned char scsi_keyword_put_sop_set_dat[20];
extern const unsigned char scsi_keyword_model_fi_6670dj[20];
extern const unsigned char scsi_keyword_model_fi_6770dj[20];
extern const unsigned char scsi_keyword_model_fi_6750sj[20];
extern const unsigned char scsi_keyword_get_dev_log_len[20];
extern const unsigned char scsi_keyword_get_dev_log_dat[20];
extern const unsigned char scsi_keyword_debug_e2p_wrt_x[20];
extern const unsigned char scsi_keyword_debug_e2t_wrt_x[20];
extern const unsigned char scsi_keyword_debug_e2t_red_x[20];
extern const unsigned char scsi_keyword_set_powoff_time[20];
extern const unsigned char scsi_keyword_get_powoff_time[20];
extern const unsigned short scsi_testmode_reply_lens[][2];
extern const unsigned short scsi_testmode_lens_column[48][2];

/* I/O */
extern volatile unsigned char mdc_version_1[];
extern volatile unsigned char mdc_version_2[];
extern volatile unsigned char mdc_version_3[];
extern volatile unsigned char mdc_imprinter_version[];
extern volatile unsigned char mdc_imprinter_version_1[];
extern volatile unsigned char mdc_imprinter_version_2[];
extern volatile unsigned char mdc_imprinter_version_3[];
extern unsigned char mdc_back_window[];

#endif
