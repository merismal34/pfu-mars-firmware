/* boot.h: the declarations of the boot programs (their files do not include fw.h). */
#ifndef BOOT_H
#define BOOT_H

/* the chip's peripheral structs (7144s.h); the ROM pointers to them are declared below */
#include "periph.h"

/* the protocol constants shared with the boot programs */
#include "scsi_defs.h"
#include "usb_defs.h"
#include "panel_defs.h"

/* fw_update_diag_request: program 2's SEND DIAGNOSTIC request for READ DIAGNOSTIC (the application's SCSI_DIAG_REQUEST_* values) */
#define FW_UPDATE_DIAG_REQUEST_BLOCK_TABLE     16
#define FW_UPDATE_DIAG_REQUEST_GET_FIRMVERSION 49

/* types: bitfield bytes (b7 is the most significant bit) */
struct Bits {
    unsigned char b7:1,
                  b6:1,
                  b5:1,
                  b4:1,
                  b3:1,
                  b2:1,
                  b1:1,
                  b0:1;
};
/* bytes stored whole and tested as bits (volatile) */
union VBF { volatile unsigned char v; volatile struct Bits f; };
/* USB controller register 0x13 (+39, USB_Control) */
struct usb_control_bits {
    unsigned char dis_bus_detect:1,
                  :1,
                  in_suspend:1,
                  :2,
                  restore_usb:1,
                  go_chirp:1,
                  active_usb:1;
};
/* fw_update_hostif_flags: the host interface program 2 serves (boot_main from the restart arguments,
   fw_update_main by VBUS); not the layout of the application's host interface flags */
struct view_fw_update_hostif_flags_bits {
    unsigned char :4,
                  scsi:1,
                  usb:1,
                  :2;
};
/* fw_update_usb_sie_status: the SIE interrupt bits collected from the controller's register 0x01 (the application's
   bodies) */
struct view_usb_sie_status_bits {
    unsigned char vbus_changed:1,
                  non_j:1,
                  bus_reset:1,
                  suspend:1,
                  :1,
                  chirp_done:1,
                  restore_done:1,
                  :1;
};
union view_usb_sie_status {
    unsigned char v;
    struct view_usb_sie_status_bits f;
};
/* fw_update_usb_transfer_flags: the state of the bulk transfer (the application's body) */
struct view_usb_transfer_flags_bits {
    unsigned char done:1,
                  :5,
                  pending:1,
                  pisces_in:1;
};

/* register block reached through the pointers at fw_update_current_record and fw_update_disconnected_record: one type
   (fw_update_queue_record takes either); from 0x38 on the names are hex offsets. t0 holds a block pointer
   (`r->t0 = fw_update_disconnected_record.p`). */
union CB { char c; struct Bits b; };
struct fw_update_command_link {
    struct fw_update_command_link *next;
    char pad_04;
    char handler;
    char saved_handler;
    char saved_handler_outer;
    char pad_08;
    unsigned char initiator_id;
    char lun[1];
    struct Bits lun_flags;
    union CB phase_flags;
    struct Bits status_flags;
    struct Bits disconnect_flags;
    char pad_0f;
    unsigned char identify;
    unsigned char message_in;
    unsigned char message_out;
    char status_byte;
    unsigned char cdb[1];
    char pad_15[11];
    unsigned char sense_key;
    char pad_21[3];
    volatile unsigned char *data_ptr;
    unsigned char *rx_ptr;
    unsigned int transfer_len;
    int data_out_count;
    char pad_34[2];
    char c54;
    char pad_37;
    char c38;
    char retries;
    unsigned char message_out_len;
    char c3b;
    char pad_3c;
    union CB mainint;
    union CB scsiint1;
    union CB scsiint2;
    char scsictl;
    char command;
};
/* the pointers as one-member structs */
struct cell_fw_update_current_record { struct fw_update_command_link *p; };
struct cell_fw_update_disconnected_record {
    struct fw_update_command_link *p;
};

/* peripheral blocks: the boot's pointers to the chip's register structs, volatile as in fw.h. boot_p_sbycr (0xffff8614, a long at +8) has no header type. */
extern volatile struct st_pfc *boot_p_pfc;
extern volatile struct st_pa *boot_p_pa;
extern volatile struct st_pb *boot_p_pb;
extern volatile struct st_pe *boot_p_pe;
extern volatile struct st_pf *boot_p_pf;
extern volatile struct st_bsc *boot_p_bsc;
extern volatile struct st_iic *boot_p_iic;

struct power_down_view { int pad_00[2]; unsigned int mstcr; };
struct fw_update_usb_config_set {
    unsigned char bLength;
    unsigned char x;
    unsigned char wTotalLength_lo;
    unsigned char pad_03[4];
};
/* USB descriptor bytes read by name (the first byte value first): a record whose first member is that byte */
struct view_fw_update_usb_device_descriptor {
    unsigned char v;
    unsigned char pad_01[6];
};
struct view_fw_update_usb_desc_max_packet0 {
    unsigned char v;
    unsigned char pad_01[10];
};
struct view_fw_update_usb_qualifier_descriptor {
    unsigned char v;
    unsigned char pad_01[27];
};
struct fw_update_usb_config_view {
    unsigned char v;
    unsigned char max_power_endpoints[20];
};
struct mtu2_view {
    char p[4];
    struct Bits tier;
    struct Bits tsr;
    short tcnt;
};
struct cmt_view { short cmstr; short cmcsr0; short cmcnt0; };
struct fw_update_cmd_entry { char opcode; char p[3]; void (*fn) (void); };
struct mtu_view { char tstr; union CB tsyr; };
struct boot_info_words { unsigned short version, sum, pad_04[6]; };
struct fw_update_sense_entry {
    unsigned short error_mask;
    short pad_02;
    unsigned long v;
};
struct fw_update_record_slot { char c; char pad_01[67]; };
/* Pisces +0x000 (fw_update_p_pisces) and the control word +0x002 (fw_update_p_pisces_ctrl): the application's views
   of the same registers */
union pisces_global_reg {
    unsigned short w;
    struct { unsigned short :15, b0:1; } n;
    struct {
        unsigned short p:8,
                       a:2,
                       b:2,
                       c:3,
                       d:1;
    } m;
};
struct pisces_ctrl_bits {
    unsigned short :6,
                   f8:2,
                   :3,
                   sdram_window:1,
                   :1,
                   hostif_stop:1,
                   :1,
                   jpeg_access:1;
};
struct fw_update_command_record {
    int p;
    unsigned char in_use;
    char pad_05[63];
};
struct fw_update_command_view_1 {
    char pad_00[5];
    char handler;
    char saved_handler;
};
struct fw_update_command_view_2 {
    char p[10];
    char lun;
    struct Bits lun_flags;
    char pad_0c;
    struct Bits status_flags;
    char q[6];
    char cdb[16];
    char *data_ptr;
    char *rx_ptr;
};
struct view_fw_update_erase_bits { char ebr1; char ebr2; };
struct fw_update_command_view_3 {
    char p[4];
    unsigned char in_use;
    unsigned char handler;
};
struct sci1_view { char smr; char brr; union CB scr; };
/* the board-control block (fw_update_p_board_ctrl): program 2's copy of hw_selftest.h's struct board_ctrl_regs and its
   per-word bit types */
struct board_ctrl_word_02 {
    unsigned short :4,
                   standby:1,
                   :1,
                   hostif_idle:1,
                   :9;
};
struct board_ctrl_word_04 { unsigned short :6, hostif_idle_enable:1, :9; };
struct board_ctrl_word_06 {
    unsigned short :5,
                   mdc_enable:1,
                   b9:1,
                   b8:1,
                   :8;
};
struct board_ctrl_word_08 {
    unsigned short :4,
                   b11:1,
                   b10:1,
                   b9:1,
                   b8:1,
                   :8;
};
struct board_ctrl_word_0a {
    unsigned short :6,
                   power_enable_9:1,
                   power_enable_8:1,
                   :8;
};
struct board_ctrl_word_0c {
    unsigned short :4,
                   b11:1,
                   cga_reset:1,
                   :1,
                   cga_select:1,
                   :8;
};
struct board_ctrl_word_0e { unsigned short :7, ccd_300dpi_mode:1, :8; };
struct board_ctrl_regs {
    struct { unsigned short p:5, code:3, q:8; } variant;
    struct board_ctrl_word_02 reg_02;
    struct board_ctrl_word_04 reg_04;
    struct board_ctrl_word_06 reg_06;
    struct board_ctrl_word_08 reg_08;
    struct board_ctrl_word_0a reg_0a;
    struct board_ctrl_word_0c reg_0c;
    struct board_ctrl_word_0e reg_0e;
};
struct flash_ctrl_view {
    struct Bits flmcr1;
    char pad_01;
    char ebr1;
    char ebr2;
};
struct cell_fw_update_queue_next { int v; };
struct fw_update_sync_entry { unsigned char v; char pad_01; };
union fw_update_usb_int_bits { unsigned char v; struct Bits f; };
struct cell_fw_update_diag_reply_len { unsigned short v; };
struct cell_fw_update_write_block_index { unsigned int v; };
struct cell_fw_update_write_addr { int v; };
struct cell_fw_update_status { unsigned short v; };
struct cell_fw_update_session_closed { unsigned short v; };
struct cell_fw_update_diag_read_len { volatile unsigned int v; };
struct cell_fw_update_diag_mode { unsigned char v; };
struct cell_fw_update_display_ticks { volatile unsigned short v; };
struct cell_fw_update_display_last_tick { volatile unsigned short v; };
struct cell_fw_update_scsi_phase { unsigned char v; };
struct cell_fw_update_scsi_prev_phase { unsigned char v; };
struct cell_fw_update_scsi_message_from { unsigned char v; };

struct fw_update_command_sense_bytes {
    char p[32];
    unsigned char sense_key;
    char pad_21;
    char asc;
    char ascq;
};

union byte_bits { unsigned char c; struct Bits b; };
struct nibbles { unsigned char h:4, l:4; };
union scsi_syncmode_view { unsigned char c; struct nibbles f; };
struct scsi_regs {
    char pad_00;
    union byte_bits mainint;
    char pad_02;
    unsigned char scsiint1;
    char pad_04;
    union byte_bits scsiint2;
    char pad_06[9];
    char reset;
    char pad_10[3];
    struct Bits scsimode0;
    char pad_14;
    struct Bits scsimode1;
    char pad_16;
    union byte_bits scsictl;
    char pad_18[3];
    union scsi_syncmode_view syncmode;
    char pad_1c[3];
    unsigned char sdid;
    char pad_20[3];
    struct Bits fifoctl;
    char pad_24;
    unsigned char fifodata;
    char pad_26;
    unsigned char ndmasize;
    char pad_28;
    unsigned char command;
    char pad_2a[5];
    union byte_bits dmactl;
    char pad_30[3];
    unsigned char dtbc2;
    char pad_34;
    unsigned char dtbc1;
    char pad_36;
    unsigned char dtbc0;
};
struct cmt1_view { unsigned char cmcsr_h; struct Bits cmcsr_l; };
struct fw_update_usb_config_bytes {
    unsigned char bLength;
    unsigned char pad_01;
    unsigned char bEndpointAddress;
    unsigned char pad_03;
    unsigned char wMaxPacketSize_lo;
    unsigned char pad_05[3];
};
struct fw_update_usb_interface_view {
    unsigned char bLength;
    char pad_01[2];
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    char pad_05[7];
    struct fw_update_usb_config_bytes *p;
};
struct usb_regs {
    char reg_00_03[9];
    unsigned char epa_int_stat;
    char pad_0a;
    unsigned char epb_int_stat;
    char reg_06_0b[13];
    char epa_int_enb;
    char pad_1a;
    char epb_int_enb;
    char pad_1c;
    char reg_0e;
    char reg_0f_13[11];
    struct Bits usb_status;
    char reg_15[3];
    struct Bits usb_test;
    char pad_2e;
    struct Bits reg_17;
    char q[37];
    struct Bits ep0_control_in;
    char pad_56;
    struct Bits ep0_control_out;
    char pad_58;
    struct Bits epa_control;
    char pad_5a;
    struct Bits epb_control;
    char pad_5c;
    struct Bits epc_control;
    char reg_2f_30[5];
    struct Bits reg_31;
    char reg_32_33[5];
    char reg_34;
    char pad_6a;
    unsigned char reg_35;
    char pad_6c;
    char reg_36;
    char pad_6e;
    char fifo_byte;
};
struct view_fw_update_port_e_bits {
    unsigned char :1,
                  ext_flash_ready:1,
                  b5:1,
                  :5;
};
struct view_fw_update_port_e {
    char pad_00;
    struct view_fw_update_port_e_bits lo;
};
union view_fw_update_record_queue {
    int v;
    struct fw_update_command_view_3 *p;
};
struct cell_fw_update_display_step { volatile unsigned short v; };

/* functions */
extern void boot_run_updater(int scsi, char sync_mode, char ultra);
extern void boot_reenter(char sync_mode, char ultra);
extern void boot_reset(void);
extern void boot_start_app(char a, char b);
extern void boot_main(unsigned int update, int scsi, char sync_mode, char ultra);
extern void boot_init_bus_ports(void);

extern void boot_clear_iram(void);
extern void boot_start_updater(unsigned int update);
extern int boot_check_sums(void);
extern int boot_iic_init_panel(void);
extern void boot_iic_write_byte(unsigned char byte);
extern int boot_iic_wait_bus_free(void);
extern int boot_iic_wait(unsigned char mode);
extern void boot_iic_clear_iric(void);
extern void boot_iic_read_bytes(unsigned char *dst, int count);
extern unsigned char boot_iic_panel_read_buttons(void);
extern void boot_isr_default(void);
extern void fw_update_restart_boot();
extern void fw_update_mem_clear(unsigned char *, int);
extern void fw_update_mem_clear_fast();
extern void fw_update_mem_copy(void *, void *, int);
extern int fw_update_mem_equal();
extern int fw_update_divu16(int, int);
extern void fw_update_usb_ep0_run(void);
extern void fw_update_usb_ep0_setup(void);
extern void fw_update_usb_ep0_data_in(void);
extern void fw_update_usb_ep0_wait_in(void);
extern void fw_update_usb_ep0_wait_status(void);
extern void fw_update_usb_ep0_data_out(void);
extern void fw_update_usb_ep0_stall(void);
extern void fw_update_usb_ep0_status(void);
extern void fw_update_usb_ep0_status_step_1(void);
extern void fw_update_finish_record(struct fw_update_command_link *record);
extern int fw_update_queue_record(struct fw_update_command_link *record);
extern void fw_update_unqueue_record(struct fw_update_command_link *record);
extern struct fw_update_command_record *fw_update_alloc_record(void);
extern void fw_update_free_record(struct fw_update_command_record *record);
extern void fw_update_release_record(struct fw_update_command_record *record);
extern void fw_update_set_current_record(struct fw_update_command_link *record);
extern void fw_update_clear_current_record(struct fw_update_command_link *record);
extern short fw_update_erase_block(unsigned int addr);
extern short fw_update_program_128(unsigned int dst_addr, unsigned int src_addr);
extern void fw_update_program_pulse(unsigned short pulse, char skip);
extern void fw_update_begin_session(void);
extern void fw_update_end_session(void);
extern void fw_update_report_blocks(unsigned char *p);
extern void fw_update_write_info_block(void);
extern void fw_update_write_block(void);
extern short fw_update_erase_ext_sector(unsigned short *addr, unsigned int mask);
extern short fw_update_program_ext(unsigned short *dst, unsigned short *src);
extern void fw_update_flash_read_mode(void);
extern void fw_update_isr_nmi(void);
extern void fw_update_isr_general_illegal(void);
extern void fw_update_isr_slot_illegal(void);
extern void fw_update_isr_address_error(void);
extern void fw_update_isr_dma_address_error(void);
extern void fw_update_isr_user_break(void);
extern void fw_update_isr_unused(void);
extern void fw_update_dispatch_scsi_event(void);
extern void fw_update_scsi_event_00(int record);
extern void fw_update_scsi_ack_interrupts(void);
extern void fw_update_scsi_event_default(int record);
extern void fw_update_scsi_read_status(struct fw_update_command_link *record);
extern int fw_update_scsi_event_unknown(struct fw_update_command_link *record);
extern int fw_update_scsi_hook_cmd_error(struct fw_update_command_link *record);
extern void fw_update_scsi_bus_reset(struct fw_update_command_link *record);
extern void fw_update_scsi_release_bus(struct fw_update_command_link *record);
extern void fw_update_scsi_disconnected(struct fw_update_command_view_1 *record);
extern void fw_update_scsi_selected(void);
extern void fw_update_scsi_read_opcode(struct fw_update_command_link *record);
extern void fw_update_scsi_receive_cdb(struct fw_update_command_link *record);
extern void fw_update_scsi_reselected(struct fw_update_command_link *record);
extern void fw_update_scsi_data_out_done(struct fw_update_command_link *record);
extern void fw_update_scsi_data_in_done(struct fw_update_command_link *record);
extern void fw_update_scsi_event_1a(struct fw_update_command_link *record);
extern void fw_update_scsi_receive_message(struct fw_update_command_link *record);
extern void fw_update_scsi_message_in_done(struct fw_update_command_link *record);
extern int fw_update_scsi_cdb_len(struct fw_update_command_link *record, unsigned char *len);
extern int fw_update_scsi_read_fifo(int unused);
extern int fw_update_scsi_start_dma(struct fw_update_command_link *record);
extern int fw_update_scsi_send_message(struct fw_update_command_link *record);
extern void fw_update_scsi_next_message(void);
extern int fw_update_scsi_send_status(struct fw_update_command_link *record);
extern int fw_update_scsi_reselect(struct fw_update_command_link *record);
extern int fw_update_scsi_receive_data(struct fw_update_command_link *record);
extern int fw_update_scsi_send_data(struct fw_update_command_link *record);
extern void fw_update_scsi_unused_hook(void);
extern int fw_update_scsi_negotiate_sync(struct fw_update_command_link *record);
extern void fw_update_scsi_set_sync(void);
extern int fw_update_scsi_set_async(void);
extern void fw_update_scsi_ctrl_command(unsigned char cmd);
extern void fw_update_scsi_take_identify(void);
extern void fw_update_isr_irq3(void);
extern void fw_update_isr_cmi1(void);
extern void fw_update_usb_read_irq_status(void);
extern void fw_update_main(int update);
extern void fw_update_poll(void);
extern void fw_update_set_vbr(void);
extern void fw_update_init_intc(int update);
extern void fw_update_init_timers(void);
extern int fw_update_init_ports(int update);
extern void fw_update_shutdown(void);
extern void fw_update_run_event_handler(void);
extern void fw_update_handle_default(int record);
extern int fw_update_main_step_5(void);
extern int fw_update_main_step_5_1(void);
extern void fw_update_main_step_5_3(int enable, int scsi);
extern int fw_update_panel_init(void);
extern void fw_update_iic_write_byte(unsigned char byte);
extern int fw_update_iic_wait_bus_free(void);
extern int fw_update_iic_wait(unsigned char phase);
extern void fw_update_iic_clear_iric(void);
extern int fw_update_panel_set_digit(unsigned char digit);
extern unsigned int fw_update_usb_run_std_request(void);
extern int fw_update_usb_req_get_status(void);
extern int fw_update_usb_req_clear_feature(void);
extern int fw_update_usb_req_set_feature(void);
extern int fw_update_usb_req_set_address(void);
extern int fw_update_usb_req_get_descriptor(void);
extern int fw_update_usb_req_get_configuration(void);
extern int fw_update_usb_req_set_configuration(void);
extern int fw_update_usb_req_get_interface(void);
extern int fw_update_usb_req_set_interface(void);
extern int fw_update_usb_req_unsupported(void);
extern int fw_update_usb_reset_endpoints(unsigned char alternate);
extern void fw_update_usb_build_serial_string(unsigned int number);
extern void fw_update_handle_command(struct fw_update_command_link *record);
extern void fw_update_handle_status(struct fw_update_command_link *record);
extern void fw_update_handle_reselect(struct fw_update_command_link *record);
/* program 2 (linked and run in RAM): references to its functions hold the run address. Program 1 can't call these by name (linked apart). */
extern void fw_update_end_command(void);
extern int fw_update_requeue_record(unsigned char state);
extern void fw_update_clear_sense_bufs(void);
extern void fw_update_clear_sense(void);
extern void fw_update_set_sense(struct fw_update_command_link *record, int sense);
extern void fw_update_build_sense(struct fw_update_command_sense_bytes *record);
extern void fw_update_record_set_sense(unsigned char *record, int sense);
extern unsigned char *fw_update_get_sense_buf(void);
extern int fw_update_has_sense(void);
extern unsigned char fw_update_initiator_bit(unsigned char id);
extern unsigned char fw_update_lowest_bit(unsigned char bits);
extern void fw_update_cmd_test_unit_ready(void);
extern void fw_update_cmd_request_sense(void);
extern void fw_update_cmd_inquiry(void);
extern int fw_update_cmd_inquiry_step_5(void);
extern int fw_update_build_vpd(void);
extern void fw_update_cmd_reserve_unit(void);
extern void fw_update_cmd_release_unit(void);
extern void fw_update_cmd_send_diagnostic(void);
extern void fw_update_cmd_read_diagnostic(void);
extern void fw_update_cmd_get_hw_status(void);
extern void fw_update_fail_command(void);
extern int fw_update_check_command(void);
extern int fw_update_check_reservation(void);
extern int fw_update_check_overlapped(void);
extern int fw_update_abort_disconnected(void);
extern int fw_update_check_unit_attention(void);
extern int fw_update_check_ready(void);
extern int fw_update_take_pending_error(void);
extern void fw_update_usb_irq_device_events(void);
extern void fw_update_usb_poll_events(void);
extern void fw_update_usb_update_ep_status(void);
extern void fw_update_usb_run_events(void);
extern void fw_update_usb_run_events_6_4(void);
extern int fw_update_usb_read_vbus(void);
extern int fw_update_usb_read_line_state(void);
extern void fw_update_usb_start_receive(int len);
extern void fw_update_usb_set_transfer_len(int len);
extern void fw_update_usb_set_status_len(void);
extern void fw_update_usb_reset_state(void);
extern void fw_update_usb_clear_transfer(void);
extern void fw_update_usb_init(void);
extern void fw_update_usb_handle_attach(void);
extern void fw_update_usb_detach(void);
extern void fw_update_usb_handle_bus_reset(void);
extern void fw_update_usb_handle_suspend(void);
extern void fw_update_usb_ack_and_reset(void);
/* fw_update_start.src (prototypes from its comments). fw_update_restart_boot, fw_update_mem_clear, fw_update_mem_clear_fast,
   fw_update_mem_copy, fw_update_mem_equal, fw_update_divu16 are declared above. */
extern void fw_update_entry(void);             /* start-up: jump to fw_update_main, doesn't return */
extern void fw_update_delay_4(void);             /* delays: busy loops of 4 ... 9656 iterations */
extern void fw_update_delay_8(void);
extern void fw_update_delay_10(void);
extern void fw_update_delay_20(void);
extern void fw_update_delay_39(void);
extern void fw_update_delay_58(void);
extern void fw_update_delay_97(void);
extern void fw_update_delay_193(void);
extern void fw_update_delay_386(void);
extern void fw_update_delay_1932(void);
extern void fw_update_delay_3862(void);
extern void fw_update_delay_9656(void);
extern void fw_update_mem_copy_fast(void *dst, void *src, int n);     /* copy, by longs when aligned */
extern void fw_update_mem_set(void *p, int c, unsigned n);      /* set n bytes to c */
extern void fw_update_mem_set_fast(void *p, int c, int n);           /* set, by longs when aligned (right for c = 0 only) */
extern int fw_update_mem_compare_count(void *a, void *b, int n, int *count);   /* compare, counting equal bytes */
extern int fw_update_usb_run_command(struct fw_update_command_view_2 *record);
extern int fw_update_usb_receive_command(struct fw_update_command_view_2 *record);
extern int fw_update_usb_command_state_1(struct fw_update_command_view_2 *record);
extern int fw_update_usb_send_status(struct fw_update_command_view_2 *record);
extern int fw_update_usb_send_status_block(struct fw_update_command_view_2 *record);
extern int fw_update_usb_data_state_1(struct fw_update_command_view_2 *record);
extern int fw_update_usb_run_data(struct fw_update_command_view_2 *record);
extern int fw_update_usb_data_state_0(int record);

/* I/O (on-chip registers) */
extern unsigned char boot_firmware_version;
extern unsigned char boot_firmware_version_1[];
extern unsigned char boot_firmware_version_2[];
extern unsigned char boot_firmware_version_3[];
extern unsigned short boot_header_sum[];
extern unsigned char boot_block_version_hi;
extern unsigned char boot_block_version_lo[];
extern unsigned int *boot_iram_begin;
extern unsigned int *boot_iram_end;
extern unsigned int *boot_update_buf_begin;
extern unsigned int *boot_update_buf_end;
extern unsigned int *boot_update_code_begin;
extern unsigned int *boot_update_code_end;
extern unsigned int boot_update_vectors;
extern unsigned short *boot_onchip_blocks[1];
extern unsigned short *boot_onchip_block_ends[8];
extern unsigned short *boot_onchip_block_ends_9[7];
extern unsigned short *boot_ext_sectors[1];
extern unsigned short *boot_ext_sector_ends[18];
extern volatile struct st_pc *boot_p_pc;
extern volatile struct st_pd *boot_p_pd;
extern struct power_down_view *boot_p_sbycr;
extern void (*const fw_update_usb_ep0_stage_table[])(void);
extern const unsigned int fw_update_onchip_blocks[];
extern const unsigned int fw_update_onchip_block_ends[];
extern const unsigned int fw_update_onchip_end[];
extern const unsigned int fw_update_ext_sectors[];
extern const unsigned int fw_update_ext_sector_ends[];
extern const unsigned int fw_update_ext_end[];
extern const struct view_fw_update_erase_bits fw_update_erase_bits[];
extern const unsigned char fw_update_display_digits[];
extern const unsigned char fw_update_usb_test_packet[];
extern const struct view_fw_update_usb_device_descriptor fw_update_usb_device_descriptor;
extern const struct view_fw_update_usb_desc_max_packet0 fw_update_usb_desc_max_packet0;
extern const struct view_fw_update_usb_qualifier_descriptor fw_update_usb_qualifier_descriptor;
extern const struct fw_update_usb_config_set fw_update_usb_config_fs;
extern const struct fw_update_usb_config_view fw_update_usb_config_attributes_fs;
extern const struct fw_update_usb_interface_view fw_update_usb_interface_fs[];
extern const struct fw_update_usb_config_set fw_update_usb_config_hs;
extern const struct fw_update_usb_config_view fw_update_usb_config_attributes_hs;
extern const struct fw_update_usb_interface_view fw_update_usb_interface_hs[];
extern unsigned int (*const fw_update_usb_std_request_table[]) (void);
extern const struct fw_update_cmd_entry fw_update_cmd_table[];
extern const struct fw_update_sense_entry fw_update_hardware_sense_table[];
extern const struct fw_update_sense_entry fw_update_medium_sense_table[];
extern const unsigned char fw_update_sense_lun_not_supported[];
extern const unsigned char fw_update_inquiry_data[];
extern const unsigned char fw_update_inquiry_invalid_lun[];
extern const unsigned char fw_update_vpd_data[];
extern const unsigned char fw_update_vpd_invalid_lun[];
extern const unsigned char fw_update_status_template[];
extern const unsigned char fw_update_version_key[];
extern int (*fw_update_usb_data_states[]) (struct fw_update_command_view_2 *);
extern int (*fw_update_scsi_event_table[]) (struct fw_update_command_link *);
extern void (*fw_update_record_handler_table[]) (struct fw_update_command_view_3 *);
extern unsigned char *fw_update_usb_serial_string_ptr[];
extern struct sci1_view *fw_update_p_sci1;
extern struct mtu_view *fw_update_p_mtu;
extern struct mtu2_view *fw_update_p_mtu2;
extern unsigned short *fw_update_p_intc;
extern volatile union pisces_global_reg *fw_update_p_pisces[];
extern volatile struct pisces_ctrl_bits *fw_update_p_pisces_ctrl[];
extern volatile struct board_ctrl_regs *fw_update_p_board_ctrl;
extern struct cmt_view *fw_update_p_cmt;
extern struct cmt1_view *fw_update_p_cmt1;
extern struct flash_ctrl_view *fw_update_p_flash;
extern char *fw_update_p_wdt_read;
extern unsigned short *fw_update_p_wdt_write;
extern struct scsi_regs *fw_update_p_scsi_ctrl;
extern struct usb_regs *fw_update_p_usb_ctrl;
extern int (*fw_update_usb_command_states[]) (struct fw_update_command_view_2 *);
extern unsigned char (*fw_update_usb_status_states[]) (struct fw_update_command_view_2 *);
/* program 2's RAM (sections B2 and BI), defined in fw_update_globals.c, declared in its definition order and with its types, so that these externs lay out the sections as the definitions do */
extern unsigned char fw_update_rx_buf[];
extern char fw_update_rx_data[];
extern struct boot_info_words boot_info_block[];
extern int fw_update_cmi1_saved_imask;
extern volatile struct st_iic **fw_update_iic_ref;
extern volatile struct st_pa *fw_update_port_a;
extern volatile struct st_pb *fw_update_port_b;
extern struct view_fw_update_port_e *fw_update_port_e;
extern volatile struct st_pf *fw_update_port_f;
extern unsigned int fw_update_sync_src;
extern volatile unsigned int fw_update_sync_dst;
extern struct fw_update_command_record fw_update_records[];
extern struct fw_update_record_slot fw_update_record_in_use[];
extern union view_fw_update_record_queue fw_update_record_queue[];
extern struct cell_fw_update_queue_next fw_update_queue_next[];
extern struct cell_fw_update_current_record fw_update_current_record;
extern struct cell_fw_update_disconnected_record fw_update_disconnected_record;
extern unsigned char fw_update_unused_1[];
extern struct Bits fw_update_state_flags;
extern unsigned char fw_update_unused_2[];
extern struct Bits fw_update_record_active;
extern unsigned char fw_update_unused_3[];
extern struct Bits fw_update_command_flags;
extern unsigned char fw_update_handover_version_0[];
extern unsigned char fw_update_handover_version_1[];
extern unsigned char fw_update_handover_version_2[];
extern unsigned char fw_update_handover_version_3[];
extern unsigned char fw_update_command_buf[];
extern unsigned char fw_update_info_block_buf[];
extern char fw_update_command_byte_19[];
extern volatile unsigned char fw_update_reply_buf[];
extern volatile unsigned char fw_update_reply_byte_1[];
extern volatile unsigned char fw_update_reply_byte_2[];
extern volatile unsigned char fw_update_reply_byte_3[];
extern volatile unsigned char fw_update_reply_byte_4[];
extern volatile unsigned char fw_update_reply_byte_5[];
extern volatile unsigned char fw_update_reply_byte_6[];
extern volatile unsigned char fw_update_reply_byte_7[];
extern volatile unsigned short fw_update_reply_byte_8[];
#define fw_update_reply_byte_9 ((unsigned char *)fw_update_reply_byte_8 + 1)   /* the low byte of fw_update_reply_byte_8[0] */
extern volatile unsigned char fw_update_reply_byte_10[];
extern volatile unsigned char fw_update_reply_byte_11[];
extern unsigned char fw_update_reply_byte_12[];
extern unsigned char fw_update_reply_byte_13[];
extern unsigned char fw_update_reply_byte_14[];
extern unsigned char fw_update_reply_byte_15[];
extern unsigned char fw_update_reply_byte_98[];
extern unsigned char fw_update_diag_request;
extern struct cell_fw_update_diag_reply_len fw_update_diag_reply_len;
extern unsigned char fw_update_unused_4[];
extern unsigned short fw_update_pending_errors;
extern unsigned short fw_update_pending_code_1;
extern unsigned short fw_update_reported_code_1;
extern unsigned short fw_update_pending_code_2;
extern unsigned short fw_update_reported_code_2;
extern unsigned short fw_update_pending_code_3;
extern unsigned short fw_update_reported_code_3;
extern struct cell_fw_update_write_block_index fw_update_write_block_index;
extern volatile unsigned int fw_update_write_len;
extern unsigned int fw_update_session_spare_1;
extern unsigned int fw_update_session_spare_2;
extern unsigned int fw_update_session_spare_3;
extern unsigned int fw_update_session_active;
extern unsigned short fw_update_restart_request;
extern volatile unsigned int fw_update_write_remaining;
extern struct cell_fw_update_write_addr fw_update_write_addr;
extern unsigned int fw_update_session_spare_4;
extern volatile unsigned int fw_update_erased_mask;
extern struct cell_fw_update_status fw_update_status;
extern struct cell_fw_update_session_closed fw_update_session_closed;
extern struct cell_fw_update_diag_read_len fw_update_diag_read_len;
extern unsigned char fw_update_unused_5[];
extern unsigned short fw_update_block_table_len;
extern unsigned char fw_update_boot_version_hi;
extern unsigned char fw_update_boot_version_lo;
extern unsigned char fw_update_unused_6[];
extern char fw_update_restart_arg1;
extern char fw_update_restart_arg2;
extern unsigned char fw_update_unused_7[];
extern unsigned char fw_update_info_block[];
extern volatile unsigned int fw_update_block_len;
extern unsigned char *fw_update_block_data;
extern struct cell_fw_update_diag_mode fw_update_diag_mode;
struct cell_fw_update_block_index { volatile unsigned char v; };
extern struct cell_fw_update_block_index fw_update_block_index;
extern volatile unsigned int fw_update_write_src;
extern unsigned int fw_update_ext_cmd_addr_1;
extern unsigned int fw_update_ext_cmd_addr_2;
extern unsigned int fw_update_ext_cmd_addr_3;
extern unsigned int fw_update_ext_cmd_addr_4;
extern unsigned int fw_update_ext_cmd_addr_5;
extern unsigned int fw_update_ext_cmd_addr_6;
extern unsigned int fw_update_reprogram_data[];
extern unsigned int fw_update_additional_data[];
extern unsigned char fw_update_unused_8[];
extern volatile struct view_fw_update_hostif_flags_bits fw_update_hostif_flags;
extern struct Bits fw_update_bus_reset_seen;
extern unsigned char fw_update_unused_9[];
extern unsigned int boot_update_request;
extern unsigned char fw_update_unused_10[];
extern volatile unsigned short fw_update_tick_count;
extern struct cell_fw_update_display_ticks fw_update_display_ticks;
extern unsigned char fw_update_unused_11[];
extern struct cell_fw_update_display_last_tick fw_update_display_last_tick;
extern struct cell_fw_update_display_step fw_update_display_step;
extern volatile unsigned short fw_update_send_timeout;
extern unsigned char fw_update_unused_12[];
extern unsigned char fw_update_sense_bufs[];
extern unsigned char fw_update_sense_buf_usb[];
extern struct fw_update_sync_entry fw_update_sync_params[];
#define fw_update_sync_offsets ((struct fw_update_sync_entry *)((unsigned char *)fw_update_sync_params + 1))   /* byte 1 of each entry */
extern unsigned char fw_update_scsi_message[];
extern unsigned char fw_update_extended_message_len[];
extern unsigned char fw_update_extended_message_code[];
extern unsigned char fw_update_sdtr_period[];
extern unsigned char fw_update_sdtr_offset[];
extern volatile unsigned char fw_update_sync_initiator;
extern unsigned char fw_update_reserved_by;
extern volatile unsigned char fw_update_ua_reset_pending;
extern unsigned char fw_update_ua_reset_pending_usb;
extern unsigned char fw_update_ua_mode_changed;
extern unsigned char fw_update_ua_mode_changed_usb;
extern unsigned char fw_update_unused_13[];
extern unsigned short fw_update_error_detail;
extern struct cell_fw_update_scsi_phase fw_update_scsi_phase;
extern struct cell_fw_update_scsi_prev_phase fw_update_scsi_prev_phase;
extern struct cell_fw_update_scsi_message_from fw_update_scsi_message_from;
extern unsigned char fw_update_scsi_pad_1[];
extern unsigned char fw_update_usb_ep0_stage;
extern unsigned char fw_update_usb_bulk_state;
extern unsigned char fw_update_usb_bulk_out_state;
extern unsigned char fw_update_usb_unused_state_1[];
extern unsigned char fw_update_usb_transfer_phase;
extern struct Bits fw_update_usb_wake_events;
extern struct Bits fw_update_usb_event_flags;
extern union VBF fw_update_usb_main_status;
extern volatile union fw_update_usb_int_bits fw_update_usb_main_int_read;
extern volatile union view_usb_sie_status fw_update_usb_sie_status;
extern volatile union fw_update_usb_int_bits fw_update_usb_sie_int_read;
extern union VBF fw_update_usb_ep_int_status;
extern volatile union fw_update_usb_int_bits fw_update_usb_ep_int_read;
extern union VBF fw_update_usb_ep0_int_status;
extern volatile union fw_update_usb_int_bits fw_update_usb_ep0_int_read;
extern union VBF fw_update_usb_epa_int_status;
extern volatile union fw_update_usb_int_bits fw_update_usb_epa_int_read;
extern union VBF fw_update_usb_epb_int_status;
extern volatile union fw_update_usb_int_bits fw_update_usb_epb_int_read;
extern unsigned char fw_update_usb_unused_1[];
extern volatile unsigned char fw_update_usb_device_state;
extern unsigned char fw_update_usb_unused_2[];
extern char fw_update_usb_epa_address[];
extern char fw_update_usb_epb_address[];
extern char fw_update_usb_epc_address[];
extern struct Bits fw_update_usb_ctrl_flags;
extern unsigned char *fw_update_usb_ep0_data_ptr;
extern unsigned short fw_update_usb_ep0_wlength;
extern unsigned short fw_update_usb_ep0_done_len;
extern unsigned short fw_update_usb_ep0_data_len;
extern unsigned char fw_update_usb_bulk_wait_count;
extern unsigned char fw_update_usb_ep0_reply_buf[];
extern unsigned char fw_update_usb_ep0_reply_byte_1[];
extern unsigned char fw_update_usb_ep0_reply_byte_2[];
extern unsigned char fw_update_usb_ep0_reply_byte_3[];
extern unsigned char fw_update_usb_ep0_event;
extern unsigned char fw_update_usb_epa_event;
extern unsigned char fw_update_usb_epb_event;
extern volatile unsigned char fw_update_usb_address;
extern unsigned short fw_update_usb_ep0_max_packet;
extern unsigned char fw_update_usb_max_packet_ep0_low;
extern unsigned short fw_update_usb_epa_max_packet;
extern unsigned char fw_update_usb_max_packet_epa_low;
extern unsigned short fw_update_usb_epb_max_packet;
extern unsigned char fw_update_usb_max_packet_epb_low;
extern unsigned char fw_update_usb_alternate_setting;
extern unsigned char fw_update_usb_unused_state_2;
extern unsigned char fw_update_usb_unused_3[];
extern char fw_update_dma_armed;
extern unsigned char fw_update_dma_done;
extern unsigned int fw_update_usb_bulk_len;
extern unsigned char fw_update_usb_unused_4[];
extern unsigned char fw_update_usb_status_dma_low;
extern struct view_usb_transfer_flags_bits fw_update_usb_transfer_flags;
extern unsigned char fw_update_usb_unused_5[];

#endif
