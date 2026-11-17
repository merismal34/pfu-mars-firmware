/* usb_defs.h: the USB layer's constants (endpoint events, device states), shared by the application
   (through common.h) and boot program 2 (boot.h). */
#ifndef USB_DEFS_H
#define USB_DEFS_H

/* usb_ep0_event, usb_epa_event, usb_epb_event: the endpoint event usb_update_ep_status records */
#define USB_EVENT_OUT_ACK   1
#define USB_EVENT_OUT_ERROR 3
#define USB_EVENT_IN_ACK    4
#define USB_EVENT_IN_ERROR  6
#define USB_EVENT_DMA_DONE  7

/* the USB device state (chapter 9) of the application and of program 2 */
#define USB_STATE_DEFAULT    0
#define USB_STATE_ADDRESS    1
#define USB_STATE_CONFIGURED 2

#endif
