/* afe_usb.h: declarations of afe_usb.c. */
#ifndef AFE_USB_H
#define AFE_USB_H
#include "common.h"

/* the AFE chip select, bits 15-14 of the AFE word */
#define AFE_CHIP_0 0
#define AFE_CHIP_1 1
#define AFE_CHIP_3 3
/* the AFE (analog front end) registers, written through p_vdcc_afe[p]->data_high: chip << 6 | register << 2 | data bits 9-8 */
#define AFE_REG_CONFIG           0
#define AFE_REG_GAIN_EVEN        2
#define AFE_REG_GAIN_ODD         3
#define AFE_REG_OFFSET_EVEN      4
#define AFE_REG_OFFSET_ODD       5
#define AFE_REG_TEST_MODE        6
#define AFE_WORD_HIGH(chip, reg) ((chip) << 6 | (reg) << 2)

/* functions */
extern void afe_select_source(void);
extern void afe_write_config(int);
extern void afe_set_test_mode(int);
extern void afe_set_gain(int, int, int, unsigned short);
extern int afe_wait_write(int);
extern void usb_read_irq_status(void);
extern void usb_handle_irq(void);
extern void usb_run_events(void);
extern void usb_irq_device_events(void);
extern void usb_poll_device_events(void);
extern void usb_update_ep_status(void);
extern void usb_run_events_6_4(void);
extern void usb_handle_attach(void);
extern int usb_read_vbus(void);
extern int usb_read_line_state(void);
extern void usb_handle_detach(void);
extern void usb_handle_bus_reset(void);
extern void usb_handle_suspend(void);
extern void usb_disable_device(void);
extern void usb_start_receive(unsigned int len);
extern void usb_set_transfer_len(unsigned int);
extern void usb_set_status_len(void);
extern void usb_reset_state(void);
extern void usb_clear_transfer_state(void);
extern void usb_ack_and_reset(void);
extern int usb_start_transfer(void);

/* constants */
extern const unsigned char usb_serial_string[18];
extern const unsigned char usb_endpoints_fs[18];
extern const unsigned char usb_endpoints_hs[18];

/* peripheral blocks: the ROM pointer table 0x3edb0-0x3ee3c, Renesas' structs from
   7144s.h. volatile as in the header's own address macros: no function compiles differently without it.
   Not header types (declared in the files): p_mtu (TSTR at 0xffff8240), p_ad (both A/D units),
   p_sbycr (a long at +8). No user yet: p_mtu34, p_pc, p_pd, p_flash, p_ubc,
   p_dmac0..p_dmac3. */
extern unsigned char (*usb_bulk_send_state_table[2]) (void);

/* variables */
extern unsigned short usb_transfer_busy;
extern unsigned short usb_sie_pending;

#endif
