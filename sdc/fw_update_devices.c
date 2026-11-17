/* Device pointers of boot program 2 (section D). */
/* @section D2 00f6b0 */
#pragma section 2
/* the string descriptor "20000000" in fw_update_usb_requests.c's data (fw_update_usb_qualifier_descriptor) */
extern const unsigned char fw_update_usb_qualifier_descriptor[];
const unsigned char *fw_update_usb_serial_string_ptr[1] = {fw_update_usb_qualifier_descriptor + 10};
/* peripheral blocks and external devices */
void *fw_update_p_sci1 = (void *)0xffff81b0;   /* SCI1 */
void *fw_update_p_mtu = (void *)0xffff8240;   /* MTU.TSTR */
void *fw_update_p_mtu2 = (void *)0xffff82a0;   /* MTU2 */
void *fw_update_p_intc = (void *)0xffff8348;   /* INTC.IPRA */
void *fw_update_p_pisces[2] = {(void *)0x00c00000, (void *)0x00c20000};   /* external 0xC00000, 0xC20000 */
void *fw_update_p_pisces_ctrl[2] = {(void *)0x00c00002, (void *)0x00c20002};   /* external 0xC00002, 0xC20002 */
void *fw_update_p_board_ctrl = (void *)0x00210000;   /* external 0x210000 */
void *fw_update_p_cmt = (void *)0xffff83d0;   /* CMT */
void *fw_update_p_cmt1 = (void *)0xffff83d8;   /* CMT1 */
void *fw_update_p_flash = (void *)0xffff8580;   /* FLASH */
void *fw_update_p_wdt_read = (void *)0xffff8610;   /* WDT */
void *fw_update_p_wdt_write = (void *)0xffff8610;   /* WDT */
void *fw_update_p_scsi_ctrl = (void *)0x00200000;   /* external 0x200000 */
void *fw_update_p_usb_ctrl = (void *)0x00c0c000;   /* external 0xC0C000 */
