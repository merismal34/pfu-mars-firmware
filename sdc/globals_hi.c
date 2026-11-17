/* globals_hi.c: uninitialised data (section BH). */
/* @section BH 4192f0 */
#include "fw.h"

#pragma section H
unsigned char scsi_update_request;
union view_flash_status flash_status;
struct cell_s8 hw_restart_scsi_syncmode;
struct cell_s8 hw_restart_scsi_ultra;
int flash_cmd_addrs_set;
struct cell_u16 flash_len;
unsigned char *volatile flash_dst;
unsigned char *flash_src;
unsigned char flash_backup_header[1];
unsigned char flash_backup_header_tail[555];
unsigned char scsi_diag_update_mode;
unsigned short *flash_cmd_addr1;
unsigned short *flash_cmd_addr2;
unsigned short *flash_cmd_addr3;
unsigned short *flash_hi_cmd_addr1;
unsigned short *flash_hi_cmd_addr2;
unsigned short *flash_hi_cmd_addr3;
unsigned short dirt_patched_count;
volatile unsigned short dirt_candidate_count;
struct cell_dirt_flat_count dirt_flat_count;
unsigned short dirt_flat_low_count;
unsigned short dirt_run_count;
union view_dirt_dip_limit dirt_dip_limit;
volatile int dirt_flat_limit;
volatile int dirt_flat_limit_low;
int dirt_defects_present;
struct cell_u16 dirt_window_max;
union view_dirt_half_width dirt_half_width;
struct cell_dirt_window_ptr dirt_window_ptr;
unsigned short dirt_depth_scaled;
unsigned short dirt_depth_limit;
struct cell_u16 dirt_line_average[6];
struct cell_u16 dirt_depth_by_port[6];
struct cell_dirt_dip_limit_by_port dirt_dip_limit_by_port[7];
struct view_scsi_defect_transfer_bits scsi_defect_transfer;
unsigned char scsi_unused_7[1];
char scsi_param_buf_large[2];
unsigned short flash_defect_lists[1152];
unsigned char flash_settings_copy[512];
int main_park_pending;
#pragma section
