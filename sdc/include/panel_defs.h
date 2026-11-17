/* panel_defs.h: the operator panel's I/O expander (IIC bus addresses, registers) and its button bits,
   shared by the application (through common.h) and the boot programs (boot.h: boot program 1 reads the
   buttons for the forced update, program 2 drives the digit). */
#ifndef PANEL_DEFS_H
#define PANEL_DEFS_H

/* the panel's I/O expander on the IIC bus: bus addresses and register numbers (iic_write_byte) */
#define PANEL_EXPANDER_WRITE    0x48
#define PANEL_EXPANDER_READ     0x49
#define PANEL_EXPANDER_INPUT_0  0
#define PANEL_EXPANDER_OUTPUT_0 2
#define PANEL_EXPANDER_OUTPUT_1 3
#define PANEL_EXPANDER_CONFIG_0 6
#define PANEL_EXPANDER_CONFIG_1 7

/* the buttons in the expander's input port (iic_panel_read_buttons, boot_iic_panel_read_buttons; low active, inverted) */
#define PANEL_BUTTON_SCAN          0x01
#define PANEL_BUTTON_SEND_TO       0x02
#define PANEL_BUTTON_FUNCTION      0x08
#define PANEL_BUTTON_POWER         0x80
#define PANEL_BUTTONS_ALL          0x8B
#define PANEL_BUTTONS_FORCE_UPDATE 0x0B

#endif
