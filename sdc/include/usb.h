/* usb.h: declarations of usb_bulk.c, usb_req_dispatch.c, usb_req_status.c, usb_req_feature.c,
   usb_req_descriptor.c, usb_ep0_setup.c and 7 more files. */
#ifndef USB_H
#define USB_H
#include "common.h"

struct usb_endpoint_descriptor {
    unsigned char bLength;
    char pad_01;
    unsigned char bEndpointAddress;
    char pad_03;
    unsigned char wMaxPacketSize_lo;
    unsigned char wMaxPacketSize_hi;
    char pad_06[2];
};
struct usb_interface_descriptor {
    unsigned char bLength;
    char pad_01;
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    char pad_05[7];
    struct usb_endpoint_descriptor *endpoints;
};

/* functions */
extern int usb_bulk_send_data(void);
extern int usb_bulk_state_1(void);
extern int usb_start_data_in(void);
extern int usb_bulk_state_0(void);
extern int usb_bulk_receive_data(void);
extern int usb_bulk_receive_command(void);
extern int usb_bulk_out_wait(void);
extern int usb_check_cdb(unsigned char *cdb_len);
extern int usb_dispatch_std_request(void);
extern int usb_req_get_status(void);
extern int usb_req_clear_feature(void);
extern int usb_req_set_feature(void);
extern int usb_req_set_address(void);
extern int usb_req_get_descriptor(void);
extern int usb_req_get_configuration(void);
extern int usb_req_set_configuration(void);
extern int usb_req_get_interface(void);
extern int usb_req_set_interface(void);
extern int usb_req_unsupported(void);
extern void usb_reset_endpoints(unsigned char alternate);
extern void usb_ep0_run(void);
extern void usb_ep0_setup(void);
extern void usb_ep0_data_in(void);
extern void usb_ep0_wait_in(void);
extern void usb_ep0_wait_status(void);
extern void usb_ep0_data_out(void);
extern void usb_ep0_stall(void);
extern void usb_ep0_status(void);
extern void usb_enter_test_mode(void);
extern void usb_send_status(void);
extern void usb_send_reply_data(void);
extern int usb_receive_params(void);
extern void usb_send_cdb_alloc_len(void);
extern void usb_send_reply(unsigned int);

/* functions */
extern void usb_hook(void);
extern void usb_stop_timers(void);
extern void usb_init(void);
extern void usb_save_error_state(void);

/* constants */
extern const unsigned char usb_test_packet[54];
struct usb_device_desc {
    unsigned char bLength;
    unsigned char bDescriptorType, bcdUSB_lo, bcdUSB_hi, bDeviceClass, bDeviceSubClass, bDeviceProtocol, bMaxPacketSize0, idVendor_lo, idVendor_hi, idProduct_lo, idProduct_hi, bcdDevice_lo, bcdDevice_hi, iManufacturer, iProduct, iSerialNumber, bNumConfigurations;
};
extern const struct usb_device_desc usb_device_descriptor;
struct usb_qualifier_desc { unsigned char bLength; unsigned char pad_01[9]; };
extern const struct usb_qualifier_desc usb_device_qualifier;
extern const unsigned char usb_configuration_fs[7];
struct usb_config_attributes_view {
    unsigned char v;
    unsigned char pad_01[2];
};
extern const struct usb_config_attributes_view usb_config_attributes_fs;
extern const struct usb_interface_descriptor usb_interface_fs[1];
extern const unsigned char usb_configuration_hs[7];
extern const struct usb_config_attributes_view usb_config_attributes_hs;
extern const struct usb_interface_descriptor usb_interface_hs[1];
extern const char usb_status_template[16];
extern unsigned char (*const usb_bulk_state_table[2]) (void);
extern int (*const usb_std_request_table[13]) ();
extern void (*const usb_ep0_stage_table[7]) (void);

/* peripheral blocks: the ROM pointer table 0x3edb0-0x3ee3c, Renesas' structs from
   7144s.h. volatile as in the header's own address macros: no function compiles differently without it.
   Not header types (declared in the files): p_mtu (TSTR at 0xffff8240), p_ad (both A/D units),
   p_sbycr (a long at +8). No user yet: p_mtu34, p_pc, p_pd, p_flash, p_ubc,
   p_dmac0..p_dmac3. */
extern const unsigned char *usb_serial_string_ptr[1];
extern unsigned char (*usb_bulk_out_state_table[2]) (void);

#endif
