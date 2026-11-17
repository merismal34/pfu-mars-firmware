#include "mdc.h"

unsigned long *const hw_section_d_rom = (unsigned long *)__sectop("D");
unsigned long *const hw_section_d_start = (unsigned long *)__sectop("R");
unsigned long *const hw_section_d_end = (unsigned long *)__secend("R");
unsigned long *const hw_section_b_start = (unsigned long *)__sectop("B");
unsigned long *const hw_section_b_end = (unsigned long *)__secend("B");
unsigned char *const p_ram_vectors = (unsigned char *)0xffdc00;
unsigned char *const p_mailbox = (unsigned char *)0x200100;
unsigned char *const p_mailbox_end = (unsigned char *)0x200500;
unsigned char *const p_chip_b_ram = (unsigned char *)0x400100;
unsigned char *const p_chip_b_ram_end = (unsigned char *)0x400405;
