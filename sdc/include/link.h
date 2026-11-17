/* link.h: declarations of link_status.c, uart_link_commands.c, uart_link_params.c,
   uart_link_replies.c, link_reply.c, link_build.c. */
#ifndef LINK_H
#define LINK_H
#include "common.h"

struct link_param_bits_2_6 { unsigned char a:2, :6; };
struct link_param_bits_1_3_4 { unsigned char t:1, a:3, :4; };
struct link_param_bits_3_2_3 { unsigned char p:3, a:2, :3; };

/* functions */
extern void link_build_device_status(void);
extern void link_build_reply_18(void);

/* functions */
extern void link_cmd_get_status(void);
extern void link_cmd_get_page_info(void);
extern void link_cmd_get_capabilities(void);
extern void link_cmd_get_identity(void);
extern void link_cmd_set_halftone(void);
extern void link_cmd_set_table_data(void);
extern void link_store_table(unsigned char *src, unsigned char *table, unsigned char *loaded_flags, int len, unsigned char size_index, unsigned char bank);
extern void link_cmd_get_detected_size(void);
extern void link_cmd_get_device_status(void);
extern void link_cmd_set_imprinter(void);
extern void link_cmd_set_imprint_text(void);
extern void link_cmd_wake(void);
extern void link_cmd_set_params_x(void);
extern void link_set_window_front(void);
extern void link_set_window(int side);
extern void link_set_window_mode(int);
extern unsigned char link_get_level(int side);
extern void link_set_window_back(void);
extern void link_apply_global_params(void);
extern int link_param_x_check_0(int *more, struct Bits *param, unsigned char *left, int mode);
extern int link_param_x_check_1(int *more, struct link_param_bits_3_2_3 *param, unsigned char *left, int mode);
extern int link_param_x_check_2(int *more, struct link_param_bits_1_3_4 *param, unsigned char *left, int mode);
extern int link_param_x_check_3(int *more, int param, unsigned char *left, int mode);
extern int link_param_x_check_4(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x_check_5(int *more, struct link_param_bits_2_6 *param, unsigned char *left, int mode);
extern int link_param_x_check_6(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x_check_8(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x_check_10(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x_check_14(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x_check_18(int *more, int param, unsigned char *left, int mode);
extern int link_param_x_check_19(int *more, struct link_param_bits_1_3_4 *param, unsigned char *left, int mode);
extern int link_param_x_check_20(int *more, int param, unsigned char *left, int mode);
extern int link_param_x_check_21(int *more, int param, unsigned char *left, int mode);
extern int link_param_x_check_22(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_error();
extern int link_param_reject(int *more, int param, int left, int mode);
extern void link_cmd_set_params_z(void);
extern int link_apply_z_params(int side);
extern int link_param_z_check_0(int *more, int param, unsigned char *left, int mode);
extern int link_param_z_check_1(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_z_check_4(int *more, int param, unsigned char *left, int mode);
extern int link_param_z_check_5(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_reject_2(int *more, int param, int left, int mode);
extern int link_param_x80_check_0(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x80_check_1(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x80_check_2(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x80_check_3(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x80_check_4(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x80_check_5(int *more, char *param, unsigned char *left, int mode);
extern int link_param_x80_check_6(int *more, int param, unsigned char *left, int mode);
extern int link_param_x80_check_7(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x80_check_9(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_x80_check_10(int *more, char *param, unsigned char *left, int mode);
extern int link_param_x80_check_11(int *more, int param, unsigned char *left, int mode);
extern int link_param_y_check_0(int *more, char *param, unsigned char *left, int mode);
extern int link_param_y_check_1(int *more, char *param, unsigned char *left, int mode);
extern int link_param_y_check_2(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_y_check_3(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_y_check_4(int *more, unsigned char *param, unsigned char *left, int mode);
extern int link_param_y_check_7(int *more, int param, unsigned char *left, int mode);
extern void link_apply_side_params_front(void);
extern void link_apply_side_params_back(void);
extern void link_apply_side_params(int side);
extern void link_build_reply(void);
extern void link_build_reply_1(void);
extern void link_build_reply_none(void);
extern void link_build_reply_2(void);
extern void link_build_reply_3(void);
extern void link_build_status_bytes(void);
extern int link_build_page_info(void);
extern void link_build_page_info_side(int side);
extern void link_build_reply_11(void);
extern void link_build_capabilities(void);
extern void link_build_identity(void);
extern void link_clear_reply(void);
extern void link_reply_1(void);
extern void link_reply_2(void);
extern void link_reply_4(void);
extern void link_reply_5(void);
extern void link_reply_6(void);
extern void link_reply_7(void);
extern void link_reply_8(void);
extern void link_reply_21(void);
extern void link_reply_22(void);
extern void link_reply_23(void);
extern void link_reply_9(void);
extern void link_reply_10(void);
extern void link_reply_11(void);
extern void link_reply_13(void);
extern void link_reply_14(void);
extern void link_reply_16(void);
extern void link_reply_17(void);
extern void link_reply_18(void);
extern void link_reply_20(void);
extern void link_set_reply(char code);
extern void link_reply_busy(void);
extern void link_reply_bad_frame(void);
extern void link_reply_not_ready(void);
extern void link_cmd_nop_e8(void);
extern void link_cmd_scan_control(void);
extern void link_cmd_settings(void);
extern void link_read_settings(void);
extern void link_write_settings(void);
extern void link_save_settings(void);
extern void link_cmd_write_memory(void);
extern void link_cmd_read_memory(void);
extern void link_cmd_set_first_date(void);
extern void link_cmd_set_baud(void);
extern void link_cmd_set_settings_text(void);
extern void link_cmd_nop_f8(void);
extern int link_build_reply_4(void);
extern void link_build_reply_5(void);
extern void link_build_reply_6(void);
extern void link_build_reply_8(void);
extern void link_build_reply_21(void);
extern void link_build_reply_22(void);
extern void link_build_reply_23(void);
extern void link_build_reply_9(void);
extern void link_build_memory(void);
extern void link_build_reply_17(void);
extern void link_cmd_power(void);
extern int link_no_page_started(void);

/* constants */
extern const unsigned char link_level_table[8];
extern const unsigned char link_z_value_table[8];
extern const unsigned short link_table_data_lengths[34];
extern const unsigned char link_param_x_check_5_table[64];
extern const unsigned char link_identity_fi6770[31];
extern const unsigned char link_identity_fi5750c[34];
extern const unsigned char link_counter_format_5[8];
extern const unsigned char link_counter_format_8[5];
extern unsigned char *link_param_dest[5];
extern void (*link_param_apply_table[5]) (void);
extern int (*link_param_x_checks[26]) (int *, unsigned char *, unsigned char *, int);
extern int (*link_param_x80_checks[12]) (int *, unsigned char *, unsigned char *, int);
extern int (*link_param_y_checks[8]) (int *, unsigned char *, unsigned char *, int);
extern int (*link_param_z_checks[9]) (int *, unsigned char *, unsigned char *, int);
extern void (*link_reply_table[24]) (void);

/* I/O */
extern volatile unsigned char mdc_settings;

#endif
