/* isr.h: declarations of isr.c, isr_exception.c. */
#ifndef ISR_H
#define ISR_H
#include "common.h"

/* functions */
extern void isr_irq0(void);
extern void isr_irq1(void);
extern void isr_irq2(void);
extern void isr_irq3(void);
extern void isr_tgia_0(void);
extern void isr_tgib_0(void);
extern void isr_request_command_layer(unsigned char);
extern void isr_trigger_mtu0(unsigned char);
extern void isr_tgia_1(void);
extern void isr_tgia_2(void);
extern void isr_tgia_3(void);
extern void isr_tgia_4(void);
extern void isr_stop_cmt0(void);
extern void isr_cmi1(void);
extern void isr_cmt1_timeout(void);
extern void isr_start_uart_timeout(void);
extern void isr_stop_cmt1(void);
extern void isr_eri_1(void);
extern void isr_rxi_1(void);
extern void isr_tei_1(void);
extern void isr_txi_1(void);
extern void isr_general_illegal(void);
extern void isr_slot_illegal(void);
extern void isr_address_error(void);
extern void isr_dma_address_error(void);
extern void isr_nmi(void);
extern void isr_user_break(void);
extern void isr_unused(void);
extern void isr_reset_hostif(void);
extern void isr_stop_output_dma(void);
extern void isr_init_scsi_ctrl(void);

#endif
