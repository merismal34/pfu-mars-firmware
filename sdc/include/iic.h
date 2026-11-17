/* iic.h: declarations of iic.c, iic_eeprom_load.c. */
#ifndef IIC_H
#define IIC_H
#include "common.h"

/* functions */
extern int iic_write_byte(unsigned char);
extern int iic_wait_bus_free(void);
extern int iic_wait(unsigned char);
extern void iic_clear_iric(void);
extern void iic_read_bytes(unsigned char *buf, int count);
extern int iic_panel_set_port0(char);
extern int iic_panel_set_digit(char);
extern char iic_panel_read_buttons(void);
extern void iic_eeprom_save_settings(void);
extern void iic_unused_1(void);
extern void iic_unused_2(void);
extern void iic_eeprom_mark_saved(void);
extern int iic_eeprom_read_cell(void);
extern void iic_eeprom_write_cell(char value);
extern void iic_eeprom_save_area2(void);
extern void iic_eeprom_save_area2_range(unsigned short first, unsigned short last);
extern short iic_eeprom_read_word(void);
extern void iic_eeprom_write_word(unsigned short word);

/* functions */
extern int iic_eeprom_load_settings(void);

/* constants */
extern const unsigned char iic_eeprom_cell_defaults[256];

#endif
