/* Tables of fw_update_usb_states.c: one const table in section C, one non-const in section D. */
/* @section C2 00f228 */
/* @section D2 00f6fc */
#pragma section 2
struct P;
/* fw_update_usb_data_state_0, fw_update_usb_send_status_block and fw_update_usb_data_state_1 of fw_update_usb_states.c (the entries hold their RAM addresses); all three return int
   there (fw_update_usb_data_state_0 takes an int), so the table that fw_update_usb_states.c declares as returning unsigned
   char needs casts */
extern int fw_update_usb_data_state_0(struct P *);
extern int fw_update_usb_send_status_block(struct P *);
extern int fw_update_usb_data_state_1(struct P *);
typedef unsigned char (*FnUc)(struct P *);

/* section C, after the tables of fw_update_scsi.c */
int (*const fw_update_usb_data_states[2])(struct P *) = {fw_update_usb_data_state_0, fw_update_usb_data_state_1};
/* section D */
unsigned char (*fw_update_usb_status_states[2])(struct P *) = {(FnUc)fw_update_usb_send_status_block, (FnUc)fw_update_usb_data_state_1};
