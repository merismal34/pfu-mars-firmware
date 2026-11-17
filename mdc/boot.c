#include "boot.h"

/* Reset entry. */
#pragma entry boot_reset(sp=0xfffbee)
void boot_reset(void)
{
    long u1;
    short u2;
    unsigned char *port;
    unsigned char level = 0, bit = 0;
    short i = 0;
    unsigned char low_count = 0;
    short k = 0;

    set_imask_exr(7);
    boot_init_bus_ports();
    for (i = 0; i < 10; i++) {
        boot_delay(); boot_delay(); boot_delay(); boot_delay(); boot_delay();
    }
    port = (unsigned char *)0xffff52;
    while (1) {
        level = *port;
        bit = level & 2;
        level = bit;
        if (low_count >= 100) { low_count = 0; break; }
        if (level == 0) low_count++;
        else low_count = 0;
    }
    if (boot_main() == 0)
        while (1) ;
    main();
}

/* On-chip register init. */
void boot_init_bus_ports(void)
{
    unsigned char *pb;
    unsigned short *pw;

    pw = (unsigned short *)0xffff3c;
    *pw = 0xc99f;
    pb = (unsigned char *)0xfffed0;
    *pb = 0x6;
    pb = (unsigned char *)0xfffed1;
    *pb = 0xff;
    pw = (unsigned short *)0xfffed2;
    *pw = 0x5569;
    pb = (unsigned char *)0xfffed4;
    *pb = 0x0;
    pb = (unsigned char *)0xfffed5;
    *pb = 0x3d;
    pb = (unsigned char *)0xffff45;
    *pb = 0x0;
    pb = (unsigned char *)0xffffac;
    *pb = 0x30;
    pb = (unsigned char *)0xffff60;
    *pb = 0x9c;
    pb = (unsigned char *)0xfffeb0;
    *pb = 0xdc;
    pb = (unsigned char *)0xffff60;
    *pb = 0x9c;
    pb = (unsigned char *)0xffff61;
    *pb = 0x8;
    pb = (unsigned char *)0xfffeb1;
    *pb = 0x7f;
    pb = (unsigned char *)0xffff61;
    *pb = 0x8;
    pb = (unsigned char *)0xffff62;
    *pb = 0x0;
    pb = (unsigned char *)0xfffeb2;
    *pb = 0x21;
    pb = (unsigned char *)0xffff62;
    *pb = 0x0;
    pb = (unsigned char *)0xfffeb9;
    *pb = 0xf;
    pb = (unsigned char *)0xfffebe;
    *pb = 0x80;
    pb = (unsigned char *)0xfffebf;
    *pb = 0x1c;
}

/* Boot init, device wait loop, RAM block copy. */
int boot_main(void)
{
    unsigned char *port_reg;
    short u;
    unsigned char *port_reg_2;
    unsigned char *port;
    unsigned char level = 0, bit = 0;
    short i = 0;
    unsigned char low_count = 0;

    if (boot_check_application() == 0) {
        port_reg = (unsigned char *)0xffff61;
        *port_reg = 0;
        port_reg = (unsigned char *)0xffff60;
        *port_reg &= ~0x40;
        CALL_AS(void, boot_test_mailbox, (unsigned char))(mailbox.request);
        mailbox.version_0 = MDC_VERSION_BOOT;
        mailbox.version_1 = 'A';
        mailbox.version_2 = '0';
        mailbox.version_3 = '0';
        port_reg_2 = (unsigned char *)0xffff60;
        *port_reg_2 &= ~0x08;
        port = (unsigned char *)0xffff52;
        while (1) {
            level = *port;
            bit = level & 8;
            level = bit;
            if (low_count >= 100) { low_count = 0; break; }
            if (level == 0) low_count++;
            else low_count = 0;
        }
        boot_handshake();
        do {
            if (mailbox.startup_state) {
            } else {
                if (mailbox.startup_request == MDC_STARTUP_INIT_MECHANICS) {
                } else {
                    continue;
                }
            }
        } while (mailbox.startup_request != MDC_STARTUP_INIT_MECHANICS);
        mailbox.ready = 0xff;
        mailbox.startup_state = MDC_STARTUP_INIT_MECHANICS;
        while (mailbox.startup_request != MDC_STARTUP_RUN) {
            if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
                mailbox.cmd_state = MDC_CMD_STATE_RUNNING;
                mailbox.cmd_state = MDC_CMD_STATE_DONE;
                mailbox.cmd_result = MDC_RESULT_OK;
            }
        }
        mailbox.startup_state = MDC_STARTUP_RUN;
        while (1) {
            mailbox.error_flags.b = 0;
            mailbox.error_code = 0;
            if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
                if (mailbox.cmd_code == MDC_CMD_DOWNLOAD_START) {
                    mailbox.cmd_state = MDC_CMD_STATE_RUNNING;
                    boot_enter_download();
                } else {
                    mailbox.cmd_state = MDC_CMD_STATE_RUNNING;
                    mailbox.cmd_state = MDC_CMD_STATE_DONE;
                    mailbox.cmd_result = MDC_RESULT_OK;
                }
            }
        }
    }
    return 1;
}

void boot_delay(void)
{
    unsigned short i;
    short j;

    for (i = 0; i < 10; i++) {
        j = i;
    }
}

void boot_enter_download(void)
{
    unsigned char *src;
    unsigned char *dst;
    unsigned char *reg;
    short count;

    set_imask_exr(7);
    reg = (unsigned char *)0xfffec4; *reg = 0;
    reg = (unsigned char *)0xfffec5; *reg = 0;
    reg = (unsigned char *)0xfffec6; *reg = 0;
    reg = (unsigned char *)0xfffec7; *reg = 0;
    reg = (unsigned char *)0xfffec8; *reg = 0;
    reg = (unsigned char *)0xfffec9; *reg = 0;
    reg = (unsigned char *)0xfffeca; *reg = 0;
    reg = (unsigned char *)0xfffecb; *reg = 0;
    reg = (unsigned char *)0xfffecc; *reg = 0;
    reg = (unsigned char *)0xfffecd; *reg = 0;
    reg = (unsigned char *)0xfffece; *reg = 0;
    reg = (unsigned char *)0xffff2e; *reg = 0;
    reg = (unsigned char *)0xffff2f;
    while ((*reg & 0x81) != 0)
        *reg = 0;
    set_imask_exr(7);
    set_imask_exr(7);
    set_imask_exr(7);
    src = (unsigned char *)__sectop("PFLASH");
    dst = (unsigned char *)__sectop("PFLASHR");
    count = 0x800;
    do {
        *dst++ = *src++;
    } while (--count);
    flash_run_download();
}

/* ROM checksum checks. */
int boot_check_application(void)
{
    if (boot_check_block_sum((unsigned short *)0x110000, (unsigned short *)0x178014, 0x4000) == 0)
        return 0;
    if (boot_check_block_sum((unsigned short *)0x118000, (unsigned short *)0x178016, 0x4000) == 0)
        return 0;
    if (boot_check_block_sum((unsigned short *)0x120000, (unsigned short *)0x178018, 0x4000) == 0)
        return 0;
    if (boot_check_block_sum((unsigned short *)0x128000, (unsigned short *)0x17801a, 0x4000) == 0)
        return 0;
    if (boot_check_block_sum((unsigned short *)0x130000, (unsigned short *)0x17801c, 0x4000) == 0)
        return 0;
    if (boot_check_block_sum((unsigned short *)0x138000, (unsigned short *)0x17801e, 0x4000) == 0)
        return 0;
    return 1;
}

/* Sum of the high and low bytes of n words, compared with *q. */
int boot_check_block_sum(unsigned short *words, unsigned short *expected, unsigned short count)
{
    unsigned short i;
    unsigned long sum;
    unsigned long u;

    sum = 0;
    for (i = 0; i < count; i++) {
        sum += *words >> 8;
        sum += *words & 0xff;
        words++;
    }
    i = (unsigned short)sum;
    if (i != *expected)
        return 0;
    else
        return 1;
}

/* External RAM pattern test. */
void boot_test_mailbox(void)
{
    unsigned char *p;
    unsigned char pattern;
    unsigned char ok;

    mailbox.handshake_high = 0;
    mailbox.handshake_low = 0;
    set_imask_exr(7);
    pattern = 0xff;
    ok = 1;
    while (ok != 0 && pattern != 0xab) {
        for (p = &mailbox.startup_state; p < &mailbox.cmd_result; p++)
            *p = pattern;
        for (p = &mailbox.startup_state; p < &mailbox.cmd_result; p++) {
            if (*p != pattern) {
                ok = 0;
                break;
            }
        }
        pattern += 0xab;
    }
    set_imask_exr(0);
    if (ok == 0)
        sleep();
    mailbox.request = 0;
}

/* External device handshake. */
void boot_handshake(void)
{
    unsigned char *p;
    unsigned char byte;
    short i;
    unsigned short handshake;

    do {
        handshake = mailbox.handshake_high << 8;
        handshake += mailbox.handshake_low;
        switch (handshake) {
        case 0:
            mailbox.handshake_high = 0x55;
            mailbox.handshake_low = 0x55;
            break;
        case MDC_HANDSHAKE_MDC_UP:
            break;
        case MDC_HANDSHAKE_SDC_ANSWER:
            mailbox.version_0 = MDC_VERSION_BOOT;
            mailbox.version_1 = 'A';
            mailbox.version_2 = '0';
            mailbox.version_3 = '0';
            p = (unsigned char *)0x178000;
            for (i = 0; i < 0x20; ) {
                byte = *p++;
                if (byte == 0xff)
                    byte = 0;
                mailbox.info_table[i++] = byte;
            }
            mailbox.info_table_valid = 0x80;
            mailbox.handshake_high = 0x33;
            mailbox.handshake_low = 0x77;
            break;
        }
        handshake = mailbox.handshake_high << 8;
        handshake += mailbox.handshake_low;
    } while (handshake != MDC_HANDSHAKE_DONE);
}
