#include "boot.h"
#pragma section 2
#pragma interrupt(fw_update_isr_nmi)
void fw_update_isr_nmi(void)
{
    fw_update_restart_boot();
}
void fw_update_isr_general_illegal(void) { while (1) { } }
void fw_update_isr_slot_illegal(void) { while (1) { } }
void fw_update_isr_address_error(void) { while (1) { } }
void fw_update_isr_dma_address_error(void) { while (1) { } }
void fw_update_isr_user_break(void) { while (1) { } }
void fw_update_isr_unused(void) { while (1) { } }
