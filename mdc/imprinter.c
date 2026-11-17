#include "mdc.h"

/* ---- objects ---- */
const unsigned long imprinter_flash_sectors[18] = {
    0x00100000, 0x00108000, 0x00110000, 0x00118000, 0x00120000, 0x00128000,
    0x00130000, 0x00138000, 0x00140000, 0x00148000, 0x00150000, 0x00158000,
    0x00160000, 0x00168000, 0x00170000, 0x00178000, 0x0017a000, 0x0017c000
};
const struct imprinter_cmd_entry imprinter_cmd_table[36] = {
    { 0x01, imprinter_rx_01 },
    { 0x02, imprinter_rx_02 },
    { 0x04, imprinter_rx_04 },
    { 0x06, imprinter_rx_06 },
    { 0x08, imprinter_rx_08 },
    { 0x0a, imprinter_rx_0a },
    { 0x0c, imprinter_rx_0c },
    { 0x0e, imprinter_rx_0e },
    { 0x10, imprinter_rx_10 },
    { 0x12, imprinter_rx_12 },
    { 0x13, imprinter_rx_12 },
    { 0x14, imprinter_rx_14 },
    { 0x15, imprinter_rx_14 },
    { 0x1a, imprinter_rx_1a },
    { 0x1b, imprinter_rx_1a },
    { 0x1c, imprinter_rx_1a },
    { 0x20, imprinter_rx_20 },
    { 0x21, imprinter_rx_21 },
    { 0x22, imprinter_rx_22 },
    { 0x24, imprinter_rx_22 },
    { 0x26, imprinter_rx_26 },
    { 0x28, imprinter_rx_28 },
    { 0x29, imprinter_rx_29 },
    { 0x2a, imprinter_rx_2a },
    { 0x2c, imprinter_rx_2c },
    { 0x2d, imprinter_rx_22 },
    { 0x2e, imprinter_rx_22 },
    { 0x2f, imprinter_rx_2f },
    { 0x30, imprinter_rx_30 },
    { 0x32, imprinter_rx_32 },
    { 0x34, imprinter_rx_34 },
    { 0x38, imprinter_rx_38 },
    { 0x3a, imprinter_rx_3a },
    { 0x3c, imprinter_rx_3c },
    { 0xff, imprinter_rx_20 },
    { 0x00, 0 }
};
const struct imprinter_cmd_entry imprinter_cmd_table_7x[8] = {
    { 0x76, imprinter_rx_76 },
    { 0x78, imprinter_rx_78 },
    { 0x79, imprinter_rx_79 },
    { 0x7a, imprinter_rx_7a },
    { 0x7b, imprinter_rx_7b },
    { 0x7c, imprinter_rx_7c },
    { 0x7d, imprinter_rx_7d },
    { 0x00, 0 }
};

/* ---- functions ---- */

void imprinter_sci0_transmit(void)
{
    imprinter_sci0_send_byte();
}

void imprinter_sci0_receive(void)
{
    short u1;
    unsigned char u2;
    union byte_view ssr;
    long u3;
    unsigned short idx;
    unsigned char data;
    unsigned char cmd;

    imprinter_channel->rx_flags.w = 0;
    while (imprinter_channel->rx_flags.w == 0) {
        if (p_sci0->SSR.BIT.RDRF) {
            imprinter_channel->rx_flags.bit.b15 = 1;
            p_sci0->SSR.BIT.RDRF = 0;
        }
        ssr.b = p_sci0->SSR.BYTE;
        if (ssr.bit.b5) {
            imprinter_channel->rx_flags.bit.b1 = 1;
            p_sci0->SSR.BIT.ORER = 0;
        }
        if (ssr.bit.b4) {
            imprinter_channel->rx_flags.bit.b2 = 1;
            p_sci0->SSR.BIT.FER = 0;
        }
    }
    imprinter_channel->rx_flags.bit.b15 = 0;
    imprinter_channel->count = 0;
    data = p_sci0->RDR;
    if (p_sci0->SSR.BIT.RDRF) {
        p_sci0->SSR.BIT.RDRF = 0;
    }
    if (imprinter_channel->rx_flags.bit.b2) {
        imprinter_channel->status.bit.b4 = 1;
        if (++imprinter_channel->error_count > 0x1388) {
            imprinter_channel->status.bit.b6 = 1;
        } else if (imprinter_channel->error_count > 0x14) {
        }
        imprinter_channel_clear();
        return;
    }
    imprinter_channel->error_count = 0;
    idx = imprinter_channel->index;
    imprinter_channel->index++;
    cmd = imprinter_channel->code.byte.l;
    if (cmd == 0x34 && imprinter_byte_1 == 0x82) {
        imprinter_tx_data.b[idx] = data;
        if (imprinter_tx_data.b[0] == 5) {
            idx = ((imprinter_tx_data.b[3] << 8) | imprinter_tx_data.b[4]) + 5;
            if (idx == imprinter_channel->index) {
                imprinter_rx_complete();
            } else {
                imprinter_rx_more();
            }
            return;
        }
    }
    imprinter_packet.b[idx] = data;
    if (++idx >= 3) {
        if (cmd == 1 || cmd == 0x26 || cmd == 0x2f || cmd == 0x21) {
            if (imprinter_channel->expected_len == 0
                || (idx == imprinter_channel->expected_len && imprinter_packet.b[0] == imprinter_channel->expected_len)
                || (idx == 3 && imprinter_packet.b[0] == 3)) {
                imprinter_rx_complete();
            } else {
                imprinter_rx_more();
            }
        } else if ((cmd & 0x7f) > 0x70) {
            if (imprinter_channel->expected_len == 0 || idx == imprinter_channel->expected_len
                || (idx == 3 && imprinter_packet.b[0] == 3)) {
                imprinter_rx_complete();
            } else {
                imprinter_rx_more();
            }
        } else {
            imprinter_rx_complete();
        }
    } else {
        imprinter_rx_more();
    }
}

void imprinter_sci0_send_byte(void)
{
    unsigned char code;

    p_sci0->SCR.BIT.TIE = 0;
    p_sci0->SCR.BIT.TEIE = 0;
    if (imprinter_channel->count != 0) {
        while (1) {
            if (!p_sci0->SSR.BIT.TDRE)
                p_sci0->SCR.BIT.TE = 0;
            else
                break;
        }
        if (p_sci0->SSR.BIT.RDRF) {
            p_sci0->SSR.BIT.RDRF = 0;
        }
        p_sci0->SCR.BIT.TE = 1;
        p_sci0->TDR = *imprinter_channel->buf++;
        p_sci0->SSR.BIT.TDRE = 0;
        imprinter_channel->count--;
        if (imprinter_channel->count == 0) {
            p_sci0->SCR.BIT.TEIE = 1;
        } else {
            p_sci0->SCR.BIT.TIE = 1;
        }
    } else {
        while (1)
            if (p_sci0->SSR.BIT.TEND)
                break;
        p_sci0->SCR.BIT.TE = 0;
        code = imprinter_channel->code.byte.l;
        if (imprinter_channel->code.bit.b14) {
            p_sci0->SCR.BIT.RE = 1;
            p_sci0->SCR.BIT.RIE = 1;
        } else {
            imprinter_send_done();
            p_sci0->SCR.BIT.RE = 0;
            p_sci0->SCR.BIT.RIE = 0;
        }
        if (p_sci0->SCR.BIT.TE) {
            p_sci0->SCR.BIT.TIE = 1;
        }
    }
}

void imprinter_sci0_take_reply(void)
{
    unsigned char code;

    imprinter_channel->tx_flags.bit.b0 = 0;
    if (!imprinter_channel->code.bit.b15) {
        imprinter_send_done();
        return;
    }
    code = imprinter_channel->code.byte.l;
    imprinter_channel->code.bit.b15 = 0;
    if (code == 0) {
        imprinter_channel->count = 0;
        imprinter_send_done();
        return;
    }
    imprinter_parse_reply(code);
    while (1) {
        if (!p_sci0->SSR.BIT.TDRE)
            p_sci0->SCR.BIT.TE = 0;
        else
            break;
    }
    p_sci0->SCR.BIT.TE = 1;
    while (1)
        if (p_sci0->SSR.BIT.TDRE)
            break;
    p_sci0->TDR = *imprinter_channel->buf++;
    p_sci0->SSR.BIT.TDRE = 0;
    imprinter_channel->count--;
}

void imprinter_rx_more(void)
{
}

void imprinter_rx_complete(void)
{
    p_sci0->SCR.BIT.RE = 0;
    imprinter_channel->tx_flags.bit.b1 = 1;
    imprinter_hook_rx_complete();
}

void imprinter_sci0_clear_errors(void)
{
    unsigned char pad[3];
    union byte_view st;
    unsigned long pad2;

    st.b = p_sci0->SSR.BYTE;
    if (st.bit.b5) {
        p_sci0->SSR.BIT.ORER = 0;
    }
    if (st.bit.b4) {
        p_sci0->SSR.BIT.FER = 0;
    }
    if (st.bit.b3) {
        p_sci0->SSR.BIT.PER = 0;
    }
}

void imprinter_hook_rx_complete(void)
{
}

void imprinter_sci0_poll(void)
{
    p_sci0->SCR.BIT.TE = 0;
    p_sci0->SCR.BIT.RE = 0;
    p_sci0->SCR.BIT.RIE = 0;
    p_sci0->SCR.BIT.TIE = 0;
    if (imprinter_channel->tx_flags.bit.b1) {
        imprinter_poll_dispatch();
    }
    if (imprinter_channel->tx_flags.bit.b0) {
        imprinter_sci0_take_reply();
    }
    if (imprinter_channel->tx_flags.bit.b2) {
        imprinter_channel_reset();
    }
    imprinter_sci0_clear_errors();
    if (p_sci0->SCR.BIT.TE) {
        p_sci0->SCR.BIT.TIE = 1;
    } else {
        p_sci0->SCR.BIT.RE = 1;
    }
}

void imprinter_sci1_transmit(void)
{
    imprinter_sci1_send_byte();
}

void imprinter_sci1_receive(void)
{
    short u1;
    unsigned char u2;
    union byte_view ssr;
    long u3;
    unsigned short idx;
    unsigned char data;
    unsigned char cmd;

    imprinter_channel->rx_flags.w = 0;
    while (imprinter_channel->rx_flags.w == 0) {
        if (p_sci1->SSR.BIT.RDRF) {
            imprinter_channel->rx_flags.bit.b15 = 1;
            p_sci1->SSR.BIT.RDRF = 0;
        }
        ssr.b = p_sci1->SSR.BYTE;
        if (ssr.bit.b5) {
            imprinter_channel->rx_flags.bit.b1 = 1;
            p_sci1->SSR.BIT.ORER = 0;
        }
        if (ssr.bit.b4) {
            imprinter_channel->rx_flags.bit.b2 = 1;
            p_sci1->SSR.BIT.FER = 0;
        }
    }
    imprinter_channel->rx_flags.bit.b15 = 0;
    imprinter_channel->count = 0;
    data = p_sci1->RDR;
    if (p_sci1->SSR.BIT.RDRF) {
        p_sci1->SSR.BIT.RDRF = 0;
    }
    if (imprinter_channel->rx_flags.bit.b2) {
        imprinter_channel->status.bit.b4 = 1;
        if (++imprinter_channel->error_count > 0x1388) {
            imprinter_channel->status.bit.b6 = 1;
        } else if (imprinter_channel->error_count > 0x14) {
        }
        imprinter_channel_clear();
        return;
    }
    imprinter_channel->error_count = 0;
    idx = imprinter_channel->index;
    imprinter_channel->index++;
    cmd = imprinter_channel->code.byte.l;
    if (cmd == 0x34 && imprinter_byte_1 == 0x82) {
        imprinter_tx_data.b[idx] = data;
        if (imprinter_tx_data.b[0] == 5) {
            idx = ((imprinter_tx_data.b[3] << 8) | imprinter_tx_data.b[4]) + 5;
            if (idx == imprinter_channel->index) {
                imprinter_sci1_rx_complete();
            } else {
                imprinter_sci1_rx_more();
            }
            return;
        }
    }
    imprinter_packet.b[idx] = data;
    if (++idx >= 3) {
        if (cmd == 1 || cmd == 0x26 || cmd == 0x2f || cmd == 0x21) {
            if (imprinter_channel->expected_len == 0
                || (idx == imprinter_channel->expected_len && imprinter_packet.b[0] == imprinter_channel->expected_len)
                || (idx == 3 && imprinter_packet.b[0] == 3)) {
                imprinter_sci1_rx_complete();
            } else {
                imprinter_sci1_rx_more();
            }
        } else if ((cmd & 0x7f) > 0x70) {
            if (imprinter_channel->expected_len == 0 || idx == imprinter_channel->expected_len
                || (idx == 3 && imprinter_packet.b[0] == 3)) {
                imprinter_sci1_rx_complete();
            } else {
                imprinter_sci1_rx_more();
            }
        } else {
            imprinter_sci1_rx_complete();
        }
    } else {
        imprinter_sci1_rx_more();
    }
}

void imprinter_sci1_send_byte(void)
{
    unsigned char code;

    p_sci1->SCR.BIT.TIE = 0;
    p_sci1->SCR.BIT.TEIE = 0;
    if (imprinter_channel->count) {
        while (1) {
            if (!p_sci1->SSR.BIT.TDRE)
                p_sci1->SCR.BIT.TE = 0;
            else
                break;
        }
        if (p_sci1->SSR.BIT.RDRF)
            p_sci1->SSR.BIT.RDRF = 0;
        p_sci1->SCR.BIT.TE = 1;
        p_sci1->TDR = *imprinter_channel->buf++;
        p_sci1->SSR.BIT.TDRE = 0;
        imprinter_channel->count--;
        if (imprinter_channel->count == 0)
            p_sci1->SCR.BIT.TEIE = 1;
        else
            p_sci1->SCR.BIT.TIE = 1;
    } else {
        while (1)
    if (p_sci1->SSR.BIT.TEND)
        break;
        p_sci1->SCR.BIT.TE = 0;
        code = imprinter_channel->code.byte.l;
        if (imprinter_channel->code.bit.b14) {
            p_sci1->SCR.BIT.RE = 1;
            p_sci1->SCR.BIT.RIE = 1;
        } else {
            imprinter_send_done();
            p_sci1->SCR.BIT.RE = 0;
            p_sci1->SCR.BIT.RIE = 0;
        }
        if (p_sci1->SCR.BIT.TE)
            p_sci1->SCR.BIT.TIE = 1;
    }
}

void imprinter_sci1_take_reply(void)
{
    unsigned char code;

    imprinter_channel->tx_flags.bit.b0 = 0;
    if (!imprinter_channel->code.bit.b15) {
        imprinter_send_done();
        return;
    }
    code = imprinter_channel->code.byte.l;
    imprinter_channel->code.bit.b15 = 0;
    if (code == 0) {
        imprinter_channel->count = 0;
        imprinter_send_done();
        return;
    }
    imprinter_parse_reply(code);
    while (1) {
        if (!p_sci1->SSR.BIT.TDRE)
            p_sci1->SCR.BIT.TE = 0;
        else
            break;
    }
    p_sci1->SCR.BIT.TE = 1;
    while (1)
    if (p_sci1->SSR.BIT.TDRE)
        break;
    p_sci1->TDR = *imprinter_channel->buf++;
    p_sci1->SSR.BIT.TDRE = 0;
    imprinter_channel->count--;
}

void imprinter_sci1_rx_more(void)
{
}

void imprinter_sci1_rx_complete(void)
{
    p_sci1->SCR.BIT.RE = 0;
    imprinter_channel->tx_flags.bit.b1 = 1;
    imprinter_hook_start_tx();
}

void imprinter_sci1_clear_errors(void)
{
    unsigned char a, b, c;
    union byte_view status;
    long z;

    status.b = p_sci1->SSR.BYTE;
    if (status.bit.b5)
        p_sci1->SSR.BIT.ORER = 0;
    if (status.bit.b4)
        p_sci1->SSR.BIT.FER = 0;
    if (status.bit.b3)
        p_sci1->SSR.BIT.PER = 0;
}

void imprinter_hook_start_tx(void)
{
}

void imprinter_sci1_poll(void)
{
    p_sci1->SCR.BIT.TE = 0;
    p_sci1->SCR.BIT.RE = 0;
    p_sci1->SCR.BIT.RIE = 0;
    p_sci1->SCR.BIT.TIE = 0;
    if (imprinter_channel->tx_flags.bit.b1)
        imprinter_poll_dispatch();
    if (imprinter_channel->tx_flags.bit.b0)
        imprinter_sci1_take_reply();
    if (imprinter_channel->tx_flags.bit.b2)
        imprinter_channel_reset();
    imprinter_sci1_clear_errors();
    if (p_sci1->SCR.BIT.TE)
        p_sci1->SCR.BIT.TIE = 1;
    else
        p_sci1->SCR.BIT.RE = 1;
}

void imprinter_poll_dispatch(void)
{
    imprinter_channel->tx_flags.bit.b1 = 0;
    imprinter_rx_dispatch();
    imprinter_channel->index = 0;
    if (imprinter_channel->code.bit.b15)
        imprinter_sci0_take_reply();
    else
        imprinter_send_done();
}

void imprinter_channel_reset(void)
{
    long z;

    imprinter_channel->tx_flags.bit.b2 = 0;
    imprinter_channel->index = 0;
    imprinter_channel->status.bit.b6 = 1;
    imprinter_channel_clear();
    imprinter_channel->rx_flags.w = 0;
    imprinter_channel->index = 0;
}

void imprinter_channel_clear(void)
{
    imprinter_channel->count = 0;
    imprinter_channel->index = 0;
    imprinter_channel->code.w = 0;
    imprinter_channel->b19 = 0;
    imprinter_send_done();
}

void imprinter_parse_reply(unsigned char cmd)
{
    unsigned short code;
    short len;
    long pad1, pad2;

    imprinter_channel->code.bit.b15 = 0;
    if (cmd == 0xff) {
        imprinter_packet.b[2] = 0xff;
        imprinter_channel->count = 1;
        imprinter_channel->expected_len = 1;
        imprinter_channel->buf = &imprinter_packet.b[2];
        imprinter_channel->code.bit.b14 = 1;
        return;
    }
    code = cmd;
    len = imprinter_code_lengths[code & 0x7f];
    if (cmd > 0x80 || len == 0)
        while (1)
            ;
    imprinter_packet.b[3] = cmd;
    if ((cmd & 0x7f) > 0x70) {
        imprinter_parse_reply_step_2(cmd);
        return;
    }
    if (len != 2) {
        if (cmd == 0xa) {
            imprinter_packet.b[2] = imprinter_entry_text[0] + 2;
            NOPROTO(util_mem_copy)(&imprinter_packet.b[4], &imprinter_entry_text[1], imprinter_entry_text[0]);
        } else if (cmd == 0xc) {
            imprinter_packet.b[2] = 0x12;
            NOPROTO(util_mem_copy)(&imprinter_packet.b[4], &imprinter_entry_header.flags[0], 0x10);
        } else if (cmd == 0x8) {
            imprinter_packet.b[4] = cmd;
        } else if (cmd == 0x21) {
            imprinter_packet.b[2] = 3;
            imprinter_packet.b[4] = imprinter_byte_7;
        } else if (cmd == 0x2c) {
            imprinter_packet.b[2] = 3;
            imprinter_packet.b[4] = imprinter_byte_3 | 0x80;
        } else if (cmd == 0x30) {
            imprinter_packet.b[2] = 8;
            imprinter_packet.b[4] = 0;
            imprinter_packet.b[5] = 0x85;
        } else if (cmd == 0x34) {
            imprinter_packet.b[2] = 0xa;
            imprinter_packet.b[4] = imprinter_byte_1;
            NOPROTO(util_mem_copy)(&imprinter_packet.b[5], &imprinter_byte_2, 7);
            if (imprinter_byte_1 == 0)
                imprinter_packet.b[5] |= 0x80;
            else if (imprinter_byte_1 == 0x82)
                imprinter_channel->expected_len = 0x2000;
            else
                imprinter_channel->expected_len = 3;
            imprinter_word_1 = 0;
        } else if (cmd == 0x2d) {
            imprinter_packet.b[2] = 4;
            imprinter_packet.b[4] = imprinter_byte_4;
            imprinter_packet.b[5] = imprinter_byte_5;
        }
        imprinter_channel->count = imprinter_packet.b[2];
    } else {
        imprinter_packet.b[2] = len;
        imprinter_channel->count = len;
    }
    imprinter_channel->buf = &imprinter_packet.b[2];
    imprinter_channel->code.bit.b14 = 1;
    if (cmd == 0x26) {
        imprinter_channel->expected_len = 0xd;
    } else if (cmd == 0x2f) {
        imprinter_channel->expected_len = 0x41;
    } else if (cmd == 1) {
        imprinter_channel->expected_len = 9;
    } else if (cmd == 0x34) {
    } else if (cmd == 0x21) {
        if (imprinter_byte_7 == 2 || imprinter_byte_7 == 3)
            imprinter_channel->expected_len = 5;
        else
            imprinter_channel->expected_len = 3;
    } else {
        imprinter_channel->expected_len = 3;
    }
}

void imprinter_parse_reply_step_2(unsigned char cmd)
{
    short i;
    short flag;
    short j;
    long pad;

    flag = 0;
    i = 0;
    imprinter_channel->expected_len = 3;
    switch (cmd) {
    case 0x78:
        imprinter_packet.b[2] = 3;
        if (imprinter_records[i].r2.bit.b7) {
            imprinter_packet.b[4] = 0;
            imprinter_records[i].r2.bit.b7 = 0;
        } else {
            imprinter_packet.b[4] = 0xff;
            imprinter_records[i].r2.bit.b5 = 0;
        }
        break;
    case 0x79:
        imprinter_packet.b[2] = 6;
        imprinter_packet.b[4] = imprinter_records[i].w22.byte.h;
        imprinter_packet.b[5] = imprinter_records[i].w22.f_0_16.f;
        imprinter_packet.b[6] = imprinter_records[i].b24;
        imprinter_packet.b[7] = imprinter_records[i].b25;
        break;
    case 0x7a:
        imprinter_records[i].send_chunk = imprinter_records[0].send_remaining.l;
        imprinter_channel->count = imprinter_records[0].send_remaining.w.l + 2;
        imprinter_channel->buf = imprinter_tx_data.b;
        flag = 1;
        break;
    case 0x7b:
        j = 4;
        imprinter_tx_header[j] = 4;
        imprinter_tx_header[j + 1] = cmd;
        imprinter_tx_header[j + 2] = (imprinter_records[i].block_count << 4) >> 8;
        imprinter_tx_header[j + 3] = imprinter_records[i].block_count << 4;
        imprinter_channel->count = (imprinter_records[i].block_count << 4) + 4;
        imprinter_channel->buf = imprinter_tx_data.b;
        flag = 1;
        break;
    case 0x7c:
        imprinter_packet.b[2] = 6;
        imprinter_packet.b[4] = imprinter_records[i].w26.byte.h;
        imprinter_packet.b[5] = imprinter_records[i].w26.f_0_16.f;
        imprinter_packet.b[6] = imprinter_records[i].b28;
        imprinter_packet.b[7] = imprinter_records[i].b29;
        break;
    case 0x7d:
        imprinter_packet.b[2] = 3;
        if (imprinter_records[i].receive_remaining.l < 0xf0)
            imprinter_packet.b[4] = imprinter_records[i].receive_remaining.b[3];
        else
            imprinter_packet.b[4] = 0xf0;
        imprinter_records[i].receive_chunk.l = imprinter_packet.b[4];
        imprinter_channel->expected_len = imprinter_records[i].receive_chunk.w.l + 5;
        break;
    case 0x76:
    default:
        imprinter_packet.b[2] = 2;
        imprinter_channel->expected_len = 0xa7;
        break;
    }
    if (flag == 0) {
        imprinter_channel->count = imprinter_packet.b[2];
        imprinter_channel->buf = &imprinter_packet.b[2];
    }
    imprinter_channel->code.bit.b14 = 1;
}

void imprinter_rx_dispatch(void)
{
    struct imprinter_cmd_entry *entry;
    unsigned char code;

    code = imprinter_channel->code.byte.l;
    if ((code & 0x7f) > 0x70)
        entry = imprinter_cmd_table_7x;
    else
        entry = imprinter_cmd_table;
    while (entry->handler != 0) {
        if (entry->code == code)
            break;
        entry++;
    }
    if (entry->handler != 0) {
        if ((code & 0x7f) > 0x70)
            entry->handler(0);
        else
            entry->handler();
    } else {
        imprinter_channel->unknown_code = code;
        imprinter_rx_unknown();
    }
}

void imprinter_rx_01(void)
{
    if (imprinter_packet.b[0] == 3)
        imprinter_rx_done();
    else if (imprinter_packet.b[0] != 9)
    {
    }
    else
        NOPROTO(util_mem_copy)(&imprinter_records[0].a48[16], &imprinter_packet.b[5], 4);
}

void imprinter_rx_02(void)
{
    imprinter_rx_done();
}

void imprinter_rx_0a(void)
{
    imprinter_rx_done();
}

void imprinter_rx_0c(void)
{
    imprinter_rx_done();
}

void imprinter_rx_0e(void)
{
    imprinter_rx_done();
}

void imprinter_rx_10(void)
{
    imprinter_rx_done();
    if (imprinter_state.w24.w & 0xf18f)
    {
    } else {
    }
}

void imprinter_rx_1a(void)
{
    imprinter_rx_done();
    if (imprinter_state.w24.w & 0xf18f)
    {
    } else {
    }
}

void imprinter_rx_22(void)
{
    imprinter_rx_done();
}

void imprinter_rx_26(void)
{
    unsigned long value;

    if (imprinter_packet.b[0] == 3) {
        imprinter_rx_done();
        return;
    }
    value = (unsigned short)((imprinter_packet.b[3] << 8) | imprinter_packet.b[4]);
    value = value * 50 + imprinter_packet.b[1];
    imprinter_ink_used = value;
    value = (unsigned short)((imprinter_packet.b[7] << 8) | imprinter_packet.b[8]);
    value = value * 50;
    imprinter_long_2 = value;
    value = (unsigned short)((imprinter_packet.b[11] << 8) | imprinter_packet.b[12]);
    imprinter_long_3 = value;
}

void imprinter_rx_28(void)
{
}

void imprinter_rx_2c(void)
{
    short z;

    imprinter_rx_done();
    if (imprinter_channel->w24.w & 0x7000)
    {
    } else {
    }
}

void imprinter_rx_30(void)
{
    imprinter_rx_done();
}

void imprinter_rx_32(void)
{
}

void imprinter_rx_34(void)
{
    if (imprinter_byte_1 & 0x80) {
        imprinter_byte_1 &= ~0x80;
        if (imprinter_tx_data.b[0] == 5)
            imprinter_word_1 = (imprinter_tx_data.b[3] << 8) | imprinter_tx_data.b[4];
    } else
        imprinter_rx_done();
}

void imprinter_rx_04(void)
{
    imprinter_rx_done();
    if (imprinter_state.w24.w & 0x708f) {
    } else {
    }
}

void imprinter_rx_06(void)
{
    imprinter_rx_done();
    if (imprinter_state.w24.w & 0x708f) {
    } else {
    }
}

void imprinter_rx_08(void)
{
}

void imprinter_rx_12(void)
{
}

void imprinter_rx_14(void)
{
}

void imprinter_rx_20(void)
{
    if (imprinter_packet.b[0] != 3 && imprinter_packet.b[0] != 0xff) {
    } else {
        imprinter_state.w24.w &= 0xf6f;
        imprinter_rx_done();
    }
}

void imprinter_rx_21(void)
{
    if (imprinter_packet.b[0] == 3) {
        imprinter_rx_done();
        if ((imprinter_byte_7 >= 0x10 && imprinter_byte_7 <= 0x1f) || (imprinter_byte_7 >= 0x30 && imprinter_byte_7 <= 0x32)) {
            if (imprinter_state.w24.w & 0x708f) {
                return;
            }
        }
    } else if (imprinter_byte_7 == 2) {
        imprinter_entry_header.counter = ((unsigned long)((imprinter_packet.b[1] << 8) | imprinter_packet.b[2]) << 16) | (unsigned short)((imprinter_packet.b[3] << 8) | imprinter_packet.b[4]);
    } else if (imprinter_byte_7 == 3) {
        imprinter_byte_6 = imprinter_packet.b[1];
        imprinter_byte_8 = imprinter_packet.b[2];
        imprinter_word_2 = (imprinter_packet.b[3] << 8) | imprinter_packet.b[4];
    }
}

void imprinter_rx_29(void)
{
}

void imprinter_rx_2a(void)
{
}

void imprinter_rx_38(void)
{
}

void imprinter_rx_3a(void)
{
}

void imprinter_rx_3c(void)
{
    imprinter_rx_done();
    imprinter_status_flags.b |= 1;
    return;
}

void imprinter_rx_2f(void)
{
    unsigned long value;

    if (imprinter_packet.b[0] == 3) {
        imprinter_rx_done();
        return;
    }
    value = (unsigned short)((imprinter_packet.b[3] << 8) | imprinter_packet.b[4]);
    value = value * 50 + imprinter_packet.b[1];
    imprinter_ink_used = value;
    value = (unsigned short)((imprinter_packet.b[7] << 8) | imprinter_packet.b[8]);
    value = value * 50;
    imprinter_long_2 = value;
    value = (unsigned short)((imprinter_packet.b[11] << 8) | imprinter_packet.b[12]);
    imprinter_long_3 = value;
}

void imprinter_rx_78(short record)
{
    if (imprinter_packet.w != 0x3eb) {
        imprinter_rx_reject();
        return;
    }
    if (imprinter_records[record].state == 2) {
        if (imprinter_packet.b[2] != 0) {
            imprinter_rx_reject();
            return;
        }
        imprinter_records[record].w0 = 1;
        imprinter_flags_5.bit.b7 = 1;
        CALL_AS(void, util_mem_fill_short, (unsigned char *, unsigned short, unsigned short))(imprinter_records[record].a48, imprinter_records[record].block_count, 0);
    } else {
        if (imprinter_packet.b[2] != 0xff) {
            imprinter_rx_reject();
            return;
        }
        imprinter_records[record].w0 = 0;
        imprinter_flags_5.bit.b7 = 0;
    }
}

void imprinter_rx_79(short record)
{
    imprinter_rx_check_frame();
}

void imprinter_rx_7a(short record)
{
    if (imprinter_rx_check_frame() == 0) {
        return;
    }
    imprinter_records[record].send_remaining.l -= imprinter_records[record].send_chunk;
    imprinter_records[record].data = (unsigned char *)((unsigned long)imprinter_records[record].data + imprinter_records[record].send_chunk);
}

void imprinter_rx_7b(short record)
{
    imprinter_rx_check_frame();
}

void imprinter_rx_7c(short record)
{
    imprinter_rx_check_frame();
}

void imprinter_rx_76(short record)
{
    if (imprinter_packet.w != 0x5ea || imprinter_channel->index < 7) {
        imprinter_rx_reject();
    } else {
        imprinter_rx_76_step_2(record);
    }
}

void imprinter_rx_7d(short record)
{
    if (imprinter_packet.w != 0x5ea || imprinter_packet.b[2] != 0x80) {
        imprinter_rx_reject();
        return;
    }
    if (imprinter_channel->index != imprinter_records[record].receive_chunk.l + 5) {
        imprinter_rx_reject();
        return;
    }
    NOPROTO(util_mem_copy)(imprinter_records[record].data, &imprinter_packet.b[5], imprinter_channel->index);
    imprinter_records[record].receive_remaining.l -= imprinter_records[record].receive_chunk.l;
    imprinter_records[record].data = (unsigned char *)((unsigned long)imprinter_records[record].data + imprinter_records[record].receive_chunk.l);
}

int imprinter_rx_check_frame(void)
{
    if (imprinter_packet.w != 0x3eb || imprinter_packet.b[2] != 0) {
        imprinter_rx_reject();
        return 0;
    }
    return 1;
}

void imprinter_rx_reject(void)
{
    imprinter_channel->status.bit.b3 = 1;
    imprinter_channel->b19 = 0;
}

void imprinter_rx_unknown(void)
{
    imprinter_channel->status.bit.b7 = 1;
}

void imprinter_rx_done(void)
{
    imprinter_state.w24.w = (imprinter_packet.b[1] << 8) | imprinter_packet.b[2];
    if (imprinter_state.w24.w & 0x107) {
        imprinter_state.status.bit.b7 = 1;
    }
}

void imprinter_start_send(unsigned char unused_arg)
{
    imprinter_channel = &imprinter_state;
    imprinter_channel->tx_flags.bit.b0 = 1;
    imprinter_channel->busy = 1;
    imprinter_send_flags.bit.sending = 1;
    imprinter_hook_start_tx();
}

void imprinter_send_done(void)
{
    imprinter_channel->busy = 0;
    imprinter_send_flags.bit.sending = 0;
}

void imprinter_send_10(void)
{
    imprinter_state.w24.w &= 0xf7f;
    imprinter_send(0x10, 0);
}

void imprinter_send_26(unsigned char param)
{
    imprinter_send_wait(1, 0x26, param);
}

void imprinter_send_02(unsigned char param)
{
    imprinter_send_wait(1, 2, param);
}

void imprinter_send_wait(short wait, unsigned char code, unsigned char param)
{
    imprinter_send(code, param);
    if (wait) {
        while (imprinter_channel->busy != 0) {
        }
    }
}

void imprinter_send(unsigned char code, unsigned char param)
{
    if (mailbox.imprinter_flags.b & MDC_IMPRINTER_NOT_INSTALLED) {
    } else {
        if (imprinter_send_flags.bit.sending) {
            imprinter_tx_data.view.flags.bit.b7 = 1;
        } else {
            imprinter_state.code.byte.l = code;
            imprinter_state.code.bit.b15 = 1;
            imprinter_start_send(param);
            imprinter_tx_data.view.flags.bit.b7 = 0;
        }
    }
}

void imprinter_send_14_wait_2(short status)
{
    short unused;

    if ((status & 0x10f) != 0 || imprinter_state.status.bit.b6) {
        imprinter_send_14_wait(1, 0);
    } else if (status & 0x10) {
    } else if (status & 0x8000) {
    } else if (status & 0x4000) {
    } else if (status & 0x2000) {
    } else if (status & 0x1000) {
    } else if (status & 0x80) {
    }
}

void imprinter_run_record(unsigned char record)
{
    unsigned long unused;
    short s;
    unsigned char cmd;

    s = 0;
    switch (imprinter_records[record].state) {
    case 4:
        cmd = 0x79;
        imprinter_send(cmd, record);
        while (imprinter_channel->busy != 0) ;
        if (imprinter_channel->status.b != 0 && !imprinter_channel->status.bit.b7) {
            return;
        }
        imprinter_records[record].state = 5;
        cmd = 0x7a;
        while (imprinter_records[record].send_remaining.l != 0) {
            imprinter_send(cmd, record);
            while (imprinter_channel->busy != 0) ;
            if (imprinter_channel->status.b != 0 && !imprinter_channel->status.bit.b7) {
                return;
            }
        }
        return;
    case 5:
        cmd = 0x7a;
        break;
    case 6:
        cmd = 0x7b;
        break;
    case 7:
        cmd = 0x7c;
        imprinter_send(cmd, record);
        while (imprinter_channel->busy != 0) ;
        if (imprinter_channel->status.b != 0 && !imprinter_channel->status.bit.b7) {
            return;
        }
        imprinter_records[record].state = 8;
        cmd = 0x7d;
        while (imprinter_records[record].receive_remaining.l != 0) {
            imprinter_send(cmd, record);
            while (imprinter_channel->busy != 0) ;
            if (imprinter_channel->status.b != 0 && !imprinter_channel->status.bit.b7) {
                imprinter_records[record].receive_chunk.l = 0;
                return;
            }
        }
        return;
    case 8:
        cmd = 0x7d;
        imprinter_send(cmd, record);
        while (imprinter_channel->busy != 0) ;
        if (imprinter_channel->status.b != 0 && !imprinter_channel->status.bit.b7) {
            imprinter_records[record].receive_chunk.l = 0;
        }
        return;
    case 2:
        cmd = 0x78;
        imprinter_records[record].r2.bit.b7 = 1;
        imprinter_send(cmd, record);
        while (imprinter_channel->busy != 0) ;
        tick_timeout = 0x4e2;
        set_imask_exr(0);
        while (tick_timeout != 0) ;
        set_imask_exr(7);
        if (imprinter_channel->status.b != 0 && !imprinter_channel->status.bit.b7) {
            return;
        }
        cmd = 0x2c;
        imprinter_byte_3 = 3;
        break;
    case 3:
        cmd = 0x2c;
        imprinter_byte_3 = 0;
        imprinter_send(cmd, record);
        while (imprinter_channel->busy != 0) ;
        if (imprinter_channel->status.b != 0 && !imprinter_channel->status.bit.b7) {
            return;
        }
        tick_timeout = 0x1f4;
        set_imask_exr(0);
        while (tick_timeout != 0) ;
        set_imask_exr(7);
        cmd = 0x78;
        break;
    case 1:
        cmd = 0x76;
        break;
    case 9:
    default:
        cmd = 1;
        break;
    }
    imprinter_send(cmd, record);
    while (imprinter_channel->busy != 0) ;
}

void imprinter_rx_76_step_2(unsigned char record)
{
    imprinter_records[record].block_count = imprinter_packet.b[6];
    NOPROTO(util_mem_copy)(imprinter_records[record].blocks, &imprinter_packet.b[7], imprinter_records[record].block_count << 4);
}

void imprinter_copy_record(unsigned char *p, unsigned char record)
{
    *p++ = 0;
    *p++ = imprinter_records[record].block_count;
    NOPROTO(util_mem_copy)(p, imprinter_records[record].blocks, imprinter_records[record].block_count << 4);
}

void imprinter_send_01(unsigned char param)
{
    imprinter_send_wait(1, 1, param);
}

void imprinter_send_28(unsigned char param)
{
    imprinter_send_wait(0, 0x28, param);
}

void imprinter_send_12(short variant, unsigned char param)
{
    if (variant) {
        imprinter_send(0x13, param);
    } else {
        imprinter_send(0x12, param);
    }
}

void imprinter_send_14(short variant, unsigned char param)
{
    if (variant) {
        imprinter_send(0x15, param);
    } else {
        imprinter_send(0x14, param);
    }
}

void imprinter_send_14_wait(short variant, unsigned char param)
{
    if (variant) {
        imprinter_send_wait(1, 0x15, param);
    } else {
        imprinter_send_wait(1, 0x14, param);
    }
}

void imprinter_send_20(unsigned char param)
{
    imprinter_state.w24.w &= 0xe70;
    imprinter_send_wait(1, 0x20, param);
}

void imprinter_send_24(unsigned char param)
{
    imprinter_state.w24.byte.l &= ~0x10;
    imprinter_send_wait(1, 0x24, param);
}

void imprinter_send_21_wait(unsigned char param)
{
    imprinter_state.w24.w &= 0xe60;
    imprinter_send_wait(1, 0x21, param);
}

void imprinter_send_21(unsigned char param)
{
    imprinter_state.w24.w &= 0xe60;
    imprinter_send(0x21, param);
}

void imprinter_send_06(unsigned char param)
{
    imprinter_send(6, param);
}

void imprinter_send_34(unsigned char param)
{
    imprinter_send_wait(1, 0x34, param);
}

void imprinter_send_status(short wait, unsigned char param)
{
    unsigned char status;

    imprinter_byte_7 = 3;
    imprinter_send_21_wait(param);
    status = imprinter_send_status_step_2(param);
    if (status != 0) {
        imprinter_byte_7 = status;
        imprinter_status_flags.b |= 0x10;
        if (wait) {
            imprinter_send_21_wait(param);
        } else {
            imprinter_send_21(param);
        }
    } else {
        if (IMPRINTER_STATUS_CODE_6) {
            imprinter_send(6, param);
        } else {
            imprinter_send(4, param);
        }
        if (wait) {
            while (imprinter_channel->busy != 0) ;
        }
    }
}

unsigned char imprinter_send_status_step_2(unsigned char unused_arg)
{
    short i;
    short unused;

    for (i = 0; i < 14; i++) {
        if (imprinter_byte_6 <= imprinter_status_table[i].a) {
            break;
        }
    }
    return imprinter_status_table[i].b;
}

unsigned char imprinter_get_ink_percent(void)
{
    unsigned char percent;

    if (imprinter_state.w24.w & 0x10) {
        imprinter_status_flags.bit.ink_low = 0;
        return 0;
    }
    if (imprinter_ink_used >= 0x2580UL) {
        imprinter_status_flags.bit.ink_low = 1;
        return 0;
    }
    percent = (0x2580UL - imprinter_ink_used) * 100 / 0x2580UL;
    if (percent < 0x21) {
        imprinter_status_flags.bit.ink_low = 1;
    } else {
        imprinter_status_flags.bit.ink_low = 0;
    }
    return percent;
}

void imprinter_probe(void)
{
    tick_timeout = 0xfa;
    while (tick_timeout != 0) ;
    imprinter_probe_uart();
}

void imprinter_probe_uart(void)
{
    unsigned long u1, u2;
    unsigned char reply;
    short status;
    unsigned char byte;

    set_imask_exr(7);
    p_chip_b_uart->lcr.bit.b7 = 0;
    p_chip_b_uart->lcr.bit.b6 = 0;
    p_chip_b_uart->lcr.bit.b5 = 0;
    p_chip_b_uart->lcr.bit.b4 = 0;
    p_chip_b_uart->lcr.bit.b3 = 1;
    p_chip_b_uart->lcr.bit.b2 = 0;
    p_chip_b_uart->lcr.f_0_2.f = 3;
    p_chip_b_uart->lcr.bit.b7 = 1;
    p_chip_b_uart->rbr_thr.b = 0x9c;
    p_chip_b_uart->ier.b = 0;
    p_chip_b_uart->lcr.bit.b7 = 0;
    byte = p_chip_b_uart->rbr_thr.b;
    status = p_chip_b_uart->iir_fcr.b;
    status = p_chip_b_uart->lsr.b;
    p_chip_b_uart->rbr_thr.b = 0xff;
    set_imask_exr(0);
    while (1) {
        if (p_chip_b_uart->lsr.bit.b5) {
            break;
        }
    }
    p_chip_b_uart->ier.bit.b0 = 1;
    imprinter_state.rx_flags.w = 0;
    tick_timeout = 0xfa;
    while (imprinter_state.rx_flags.w == 0 && tick_timeout != 0) {
        byte = imprinter_line_status.b;
        if (byte & IMPRINTER_LSR_DATA_READY) {
            imprinter_state.rx_flags.bit.b15 = 1;
        }
        if (byte & IMPRINTER_LSR_OVERRUN) {
            imprinter_state.rx_flags.bit.b1 = 1;
        }
        if (byte & IMPRINTER_LSR_FRAMING_ERROR) {
            imprinter_state.rx_flags.bit.b2 = 1;
        }
        if (byte & IMPRINTER_LSR_PARITY_ERROR) {
            imprinter_state.rx_flags.bit.b0 = 1;
        }
    }
    imprinter_state.rx_flags.bit.b15 = 0;
    reply = p_chip_b_uart->rbr_thr.b;
    p_chip_b_uart->ier.bit.b0 = 0;
    if (imprinter_line_status.bit.data_ready) {
        imprinter_line_status.bit.data_ready = 0;
    }
    if (imprinter_state.rx_flags.w != 0) {
        mailbox.imprinter_flags.bit.not_installed = 1;
        mailbox.option_present = 0;
    } else if (reply == 0xff) {
        mailbox.imprinter_flags.bit.not_installed = 0;
        mailbox.option_present = MDC_OPTION_PRESENT;
    } else {
        mailbox.imprinter_flags.bit.not_installed = 1;
        mailbox.option_present = 0;
        return;
    }
}
