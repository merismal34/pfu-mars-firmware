#include "boot.h"
extern void _builtin_set_imask(int);
/* Data of boot program 1 (section D). */
   /* SBYCR at 0xffff8614, as boot_main.c */
/* boot_iram_begin/end, boot_update_buf_begin/end, boot_update_code_begin/end and boot_update_vectors
   (the first words of this data) are defined in boot_start.src from program 2's sections
   (STARTOF/SIZEOF), so they follow its link */
/* flash block boundaries, low ROM (0x0-0x40000) and high ROM (0x800000-0x880000), one table
   that boot_check_sums indexes as boot_onchip_blocks[i] / boot_onchip_block_ends[i] (= boot_onchip_blocks[i + 1]) and from 16 on
   as boot_ext_sectors[i] / boot_ext_sector_ends[i]: split here at the names it uses; the last word, 0x80000,
   is the size of the high ROM */
unsigned short *boot_onchip_blocks[1] = {
    (unsigned short *)0x0
};
unsigned short *boot_onchip_block_ends[8] = {
    (unsigned short *)0x1000, (unsigned short *)0x2000, (unsigned short *)0x3000,
    (unsigned short *)0x4000, (unsigned short *)0x5000, (unsigned short *)0x6000,
    (unsigned short *)0x7000, (unsigned short *)0x8000
};
unsigned short *boot_onchip_block_ends_9[7] = {
    (unsigned short *)0x10000, (unsigned short *)0x18000, (unsigned short *)0x20000,
    (unsigned short *)0x28000, (unsigned short *)0x30000, (unsigned short *)0x38000,
    (unsigned short *)0x40000
};
unsigned short *boot_ext_sectors[1] = {
    (unsigned short *)0x800000
};
unsigned short *boot_ext_sector_ends[18] = {
    (unsigned short *)0x808000, (unsigned short *)0x810000, (unsigned short *)0x818000,
    (unsigned short *)0x820000, (unsigned short *)0x828000, (unsigned short *)0x830000,
    (unsigned short *)0x838000, (unsigned short *)0x840000, (unsigned short *)0x848000,
    (unsigned short *)0x850000, (unsigned short *)0x858000, (unsigned short *)0x860000,
    (unsigned short *)0x868000, (unsigned short *)0x870000, (unsigned short *)0x878000,
    (unsigned short *)0x87a000, (unsigned short *)0x87c000, (unsigned short *)0x80000
};
/* the peripheral blocks (boot.h; boot_p_pc and boot_p_pd are not referenced, boot_p_pa,
   boot_p_pb and boot_p_pf only by program 2: fw_update_init_ports) */
volatile struct st_pfc *boot_p_pfc = (volatile struct st_pfc *)0xffff8384;
volatile struct st_pa *boot_p_pa = (volatile struct st_pa *)0xffff8380;
volatile struct st_pb *boot_p_pb = (volatile struct st_pb *)0xffff8390;
volatile struct st_pc *boot_p_pc = (volatile struct st_pc *)0xffff8392;
volatile struct st_pd *boot_p_pd = (volatile struct st_pd *)0xffff83a0;
volatile struct st_pe *boot_p_pe = (volatile struct st_pe *)0xffff83b0;
volatile struct st_pf *boot_p_pf = (volatile struct st_pf *)0xffff83b3;
volatile struct st_bsc *boot_p_bsc = (volatile struct st_bsc *)0xffff8620;
struct power_down_view *boot_p_sbycr = (struct power_down_view *)0xffff8614;
volatile struct st_iic *boot_p_iic = (volatile struct st_iic *)0xffff87f0;


void boot_main(unsigned int update, int scsi, char sync_mode, char ultra)
{
    unsigned short sums_bad;

    sums_bad = 0;
    _builtin_set_imask(15);
    if (update != 0)
        boot_init_bus_ports();
    boot_clear_iram();
    if (update == 0) {
        boot_iic_init_panel();
        if (boot_check_sums() == 0)
            sums_bad = 0x80;
    }
    boot_clear_iram();
    if (update != 0) {
        if (scsi != 0) {
            fw_update_hostif_flags.scsi = 1;
            fw_update_restart_arg1 = sync_mode;
            fw_update_restart_arg2 = ultra;
        } else {
            fw_update_hostif_flags.usb = 1;
        }
    }
    fw_update_ua_reset_pending = 0xff;
    fw_update_ua_reset_pending_usb = 0xff;
    if (sums_bad == 0x80)
        boot_update_request = 1;
    /* 11: three panel buttons held at power-on: start the update program */
    if (boot_iic_panel_read_buttons() == PANEL_BUTTONS_FORCE_UPDATE)
        boot_update_request = 1;
    boot_update_request |= update;
    boot_start_updater(update);
    boot_start_app(sync_mode, ultra);
}

void boot_init_bus_ports(void)
{
    boot_p_sbycr->mstcr = 0xf01cc0c0;
    boot_p_bsc->BCR1.WORD = 0x600f;
    boot_p_bsc->BCR2.WORD = 0x91;
    boot_p_bsc->WCR1.WORD = 0x5515;
    boot_p_bsc->WCR2.WORD = 0;
    boot_p_pfc->PACRH.WORD = 0;
    boot_p_pfc->PACRL1.WORD = 0x5550;
    boot_p_pfc->PACRL2.WORD = 0xac70;
    boot_p_pfc->PAIORH.WORD = 0;
    boot_p_pfc->PAIORL.WORD = 0x303;
    boot_p_pfc->PBCR1.WORD = 0xc03;
    boot_p_pfc->PBCR2.WORD = 0x2505;
    boot_p_pfc->PBIOR.WORD = 0x280;
    boot_p_pfc->PCCR.WORD = 0xffff;
    boot_p_pfc->PCIOR.WORD = 0;
    boot_p_pfc->PDCRH1.WORD = 0;
    boot_p_pfc->PDCRH2.WORD = 0;
    boot_p_pfc->PDCRL1.WORD = 0xffff;
    boot_p_pfc->PDCRL2.WORD = 0;
    boot_p_pfc->PDIORH.WORD = 0;
    boot_p_pfc->PDIORL.WORD = 0;
    boot_p_pe->DRL.WORD = 0x1000;
    boot_p_pfc->PECRL1.WORD = 0;
    boot_p_pfc->PECRL2.WORD = 0;
    boot_p_pfc->PEIORL.WORD = 0x96ac;
}

void boot_clear_iram(void)
{
    unsigned int *p;
    int x;

    for (p = boot_iram_begin; p < boot_iram_end; p++)
        *p = 0;
}

void boot_start_updater(unsigned int update)
{
    unsigned int *dst;
    unsigned int *src;
    void (*entry)(unsigned int);

    _builtin_set_imask(15);
    if (boot_update_request == 0)
        return;
    dst = (unsigned int *)boot_update_vectors;
    src = (unsigned int *)0x1000;   /* program 2's vectors in ROM (section VECT2): a number that must follow the link */
    while (dst < (unsigned int *)(boot_update_vectors + 0x400)) {
        *dst = *src;
        dst++;
        src++;
    }
    dst = boot_update_code_begin;
    src = (unsigned int *)0x1400;   /* program 2's code in ROM (section P2): a number that must follow the link */
    while (dst < boot_update_code_end) {
        *dst = *src;
        dst++;
        src++;
    }
    boot_p_pfc->PEIORL.WORD |= 0x20;
    fw_update_boot_version_hi = boot_block_version_hi;
    fw_update_boot_version_lo = boot_block_version_lo[0];
    fw_update_handover_version_0[0] = boot_firmware_version;
    fw_update_handover_version_1[0] = boot_firmware_version_1[0];
    fw_update_handover_version_2[0] = boot_firmware_version_2[0];
    fw_update_handover_version_3[0] = boot_firmware_version_3[0];
    entry = (void (*)(unsigned int))fw_update_main;   /* program 2's main, at its run address (same link) */
    entry(update);
}

int boot_check_sums(void)
{
    unsigned short *p;
    int sum;
    int i;
    int unused;
    struct boot_info_words *info;

    info = boot_info_block;
    sum = 0;
    for (p = (unsigned short *)0x1000; p < boot_onchip_block_ends_9[0]; p++) {
        sum += (unsigned char)(*p >> 8);
        sum += (unsigned char)*p;
    }
    sum = (unsigned short)sum;
    if (sum != boot_header_sum[0]) {
    }
    info += 9;
    for (i = 9; i < 15; i++) {
        sum = 0;
        for (p = boot_onchip_blocks[i]; p < *(unsigned short **)(boot_onchip_block_ends + i); p++) {
            sum += (unsigned char)(*p >> 8);
            sum += (unsigned char)*p;
        }
        sum = (unsigned short)sum;
        if (sum != info->sum)
            return 0;
        info++;
    }
    for (i = 0; i < 15; i++) {
        sum = 0;
        for (p = boot_ext_sectors[i]; p < *(unsigned short **)(boot_ext_sector_ends + i); p++) {
            sum += (unsigned char)(*p >> 8);
            sum += (unsigned char)*p;
        }
        sum = (unsigned short)sum;
        if (sum != info->sum)
            return 0;
        info++;
    }
    return 1;
}

int boot_iic_init_panel(void)
{
    boot_p_iic->SCRX.BIT.IICE = 1;
    boot_p_iic->ICCR.BIT.ICE = 1;
    boot_p_iic->ICCR.BIT.ICE = 1;
    boot_p_iic->ICSR.BYTE = 0;
    boot_p_iic->SCRX.BIT.IICX0 = 0;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.MLS = 0;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.WAIT = 0;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.CKS = 3;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.BC = 0;
    boot_p_iic->SCRX.BIT.HNDS = 1;
    boot_p_iic->ICCR.BIT.IEIC = 0;
    boot_p_iic->ICCR.BIT._MST = 0;
    boot_p_iic->ICCR.BIT.TRS = 0;
    boot_p_iic->ICCR.BIT.ACKE = 1;
    if (boot_iic_wait_bus_free() == 0)
        return 0;
    boot_p_iic->ICCR.BYTE = 0xb1;
    boot_p_iic->ICCR.BYTE = 0xb4;
    boot_iic_wait(0);
    boot_iic_write_byte(PANEL_EXPANDER_WRITE);
    boot_iic_write_byte(PANEL_EXPANDER_CONFIG_0);
    boot_iic_write_byte(0xff);
    boot_iic_write_byte(0xff);
    boot_iic_clear_iric();
    boot_p_iic->ICCR.BYTE = 0xb0;
    boot_iic_wait(3);
    boot_p_iic->ICCR.BYTE = 0xa1;
}

void boot_iic_write_byte(unsigned char byte)
{
    boot_p_iic->EQU.ICE1.UN_ICDR = byte;
    boot_iic_clear_iric();
    boot_iic_wait(1);
    return;
}

int boot_iic_wait_bus_free(void)
{
    unsigned int i;
    unsigned int j;

    for (i = 0; i < 0xffffffff; i++) {
        if (boot_p_iic->ICCR.BIT.BBSY == 0)
            return 1;
        else {
        for (j = 0; j < 0xffffffff; j++) {
            if (boot_p_iic->ICCR.BIT.BBSY == 0)
                return 1;
        }
        }
    }
    return 0;
}

int boot_iic_wait(unsigned char mode)
{
    unsigned int i;
    unsigned int j;

    if (mode == 3) {
        if (boot_iic_wait_bus_free() == 0)
            return 0;
        boot_iic_clear_iric();
        return 1;
    }
    for (i = 0; i < 0xffffffff; i++) {
        if (boot_p_iic->ICCR.BIT.IRIC)
            break;
        else {
        for (j = 0; j < 0xffffffff; j++) {
            if (boot_p_iic->ICCR.BIT.IRIC)
                break;
        }
        }
        if (i >= 0xfffffff0)
            return 0;
    }
    if (mode == 0) {
        for (i = 0; i < 0xffffffff; i++) {
            if (boot_p_iic->ICSR.BIT.IRTR)
                break;
            else {
            for (j = 0; j < 0xffffffff; j++) {
                if (boot_p_iic->ICSR.BIT.IRTR)
                    break;
            }
            }
            if (i >= 0xfffffff0)
                return 0;
        }
        boot_iic_clear_iric();
        return 1;
    } else if (mode == 1) {
        for (i = 0; i < 0xffffffff; i++) {
            if (boot_p_iic->ICSR.BIT.ACKB == 0)
                break;
            else {
            for (j = 0; j < 0xffffffff; j++) {
                if (boot_p_iic->ICSR.BIT.ACKB == 0)
                    break;
            }
            }
            if (i >= 0xfffffff0)
                return 0;
        }
        boot_iic_clear_iric();
    } else if (mode == 2) {
        for (i = 0; i < 0xffffffff; i++) {
            if (boot_p_iic->SCRX.BIT.ICDRF0 && boot_p_iic->ICCR.BIT.BBSY)
                break;
            else {
            for (j = 0; j < 0xffffffff; j++) {
                if (boot_p_iic->SCRX.BIT.ICDRF0 && boot_p_iic->ICCR.BIT.BBSY)
                    break;
            }
            }
            if (i >= 0xfffffff0)
                return 0;
        }
        boot_iic_clear_iric();
        return 1;
    }
}

void boot_iic_clear_iric(void)
{
    boot_p_iic->ICCR.BIT.IRIC = 0;
    return;
}

void boot_iic_read_bytes(unsigned char *dst, int count)
{
    char c;

    boot_p_iic->ICCR.BIT.TRS = 0;
    boot_p_iic->ICSR.BIT.ACKB = 0;
    boot_p_iic->SCRX.BIT.HNDS = 1;
    boot_iic_clear_iric();
    c = boot_p_iic->EQU.ICE1.UN_ICDR;
    boot_iic_wait(2);
    boot_iic_clear_iric();
    for (;;) {
        *dst++ = boot_p_iic->EQU.ICE1.UN_ICDR;
        boot_iic_clear_iric();
        boot_iic_wait(2);
        boot_iic_clear_iric();
        if (--count == 0) {
            boot_p_iic->ICSR.BIT.ACKB = 1;
            *dst++ = boot_p_iic->EQU.ICE1.UN_ICDR;
            boot_iic_clear_iric();
            boot_iic_wait(2);
            boot_iic_clear_iric();
            boot_p_iic->ICCR.BIT.TRS = 1;
            c = boot_p_iic->EQU.ICE1.UN_ICDR;
            break;
        }
    }
}

unsigned char boot_iic_panel_read_buttons(void)
{
    char bytes[8];

    if (boot_iic_wait_bus_free() == 0)
        return 0;
    boot_p_iic->ICCR.BYTE = 0xb1;
    boot_p_iic->ICCR.BYTE = 0xb4;
    boot_iic_wait(0);
    boot_iic_write_byte(PANEL_EXPANDER_WRITE);
    boot_iic_write_byte(PANEL_EXPANDER_INPUT_0);
    boot_p_iic->ICCR.BYTE = 0xb0;
    boot_iic_wait(3);
    boot_p_iic->ICCR.BYTE = 0xa1;
    if (boot_iic_wait_bus_free() == 0)
        return 0;
    boot_p_iic->ICCR.BYTE = 0xb1;
    boot_p_iic->ICCR.BYTE = 0xb4;
    boot_iic_wait(0);
    boot_iic_write_byte(PANEL_EXPANDER_READ);
    boot_iic_read_bytes(bytes, 2);
    bytes[0] = ~bytes[0] & PANEL_BUTTONS_FORCE_UPDATE;
    boot_p_iic->ICCR.BYTE = 0xa0;
    boot_iic_wait(3);
    boot_p_iic->ICCR.BYTE = 0xa1;
    return bytes[0];
}
