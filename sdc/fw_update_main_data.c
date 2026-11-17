/* Handler table of fw_update_main.c (256 entries), section D. */
/* @section D2 00f2b0 */
#pragma section 2
struct E8;
extern void fw_update_handle_default(struct E8 *);
extern void fw_update_handle_command(struct E8 *);
extern void fw_update_handle_status(struct E8 *);
extern void fw_update_handle_reselect(struct E8 *);
extern void fw_update_usb_run_command(struct E8 *);

void (*fw_update_record_handler_table[256])(struct E8 *) = {
    /* [0..31] */
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_status,
    fw_update_handle_reselect,
    fw_update_handle_default,
    fw_update_handle_command,
    /* [36..128] */
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_usb_run_command,
    /* [130..255] */
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default,
    fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default, fw_update_handle_default
};
