/* Table of fw_update_usb_command.c (section D). */
/* @section D2 00f6f4 */
#pragma section 2
struct P;
extern int fw_update_usb_receive_command(struct P *);
extern int fw_update_usb_command_state_1(struct P *);

int (*fw_update_usb_command_states[2])(struct P *) = {fw_update_usb_receive_command, fw_update_usb_command_state_1};
