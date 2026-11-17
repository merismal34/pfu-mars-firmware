#include "fw.h"
#pragma section H


/* Constants (section C). */
void (*const console_rx_state_table[10])(void) = {
    console_rx_state_0, console_rx_state_idle, console_rx_state_idle, console_rx_state_idle,
    console_rx_state_idle, console_rx_state_5, console_rx_state_6, console_rx_state_7,
    console_rx_state_8, console_rx_state_9
};
void (*const console_tx_state_table[11])(void) = {
    console_tx_state_idle, console_tx_state_1, console_tx_state_2, console_tx_state_3,
    console_tx_state_4, console_tx_state_5, console_tx_state_6, console_tx_state_7,
    console_tx_state_8, console_tx_state_9, console_tx_state_idle
};
struct console_cmd *const console_cmd_table[6] = {
    &console_cmd_entry_s, &console_cmd_entry_d, &console_cmd_entry_e, &console_cmd_entry_ew,
    &console_cmd_entry_r, 0
};
const struct console_cmd console_cmd_entry_s = {5, {0, 0, 0}, console_cmd_s, "S"};
const struct console_cmd console_cmd_entry_d = {6, {0, 0, 0}, console_cmd_d, "D"};
const struct console_cmd console_cmd_entry_e = {7, {0, 0, 0}, console_cmd_e, "E"};
const struct console_cmd console_cmd_entry_ew = {8, {0, 0, 0}, console_cmd_ew, "EW"};
const struct console_cmd console_cmd_entry_r = {9, {0, 0, 0}, console_cmd_r, "R"};
const unsigned char console_msg_crlf[4] = {2, 0x0d, 0x0a, 0};
const unsigned char console_msg_backspace[5] = {3, 8, 0x20, 8, 0};
const unsigned char console_msg_prompt[3] = {1, 0x3e, 0};
const unsigned char console_msg_illegal[28] = "\x1aIllegal Command Recieved\r\n";
const unsigned char console_msg_crlf_illegal[28] = "\x1a\r\nIllegal Command Recieved";
const unsigned char console_msg_crlf_illegal_crlf[30] = "\x1c\r\nIllegal Command Recieved\r\n";
const unsigned char console_msg_esc_to_exit[27] = "\x19\r\nEnter [ESC] Key to exit";
/* The SCI1 {SMR, BRR} records hw_set_sci1_baud reads (entry 0). */
const struct hw_sci_baud console_sci1_baud[3] = {{0x00, 0x13}, {0x00, 0x26}, {0x00, 0x00}};


#define REGSET(r, g, k) \
  s = g; s <<= 2; s &= 0x3ff; \
  p_vdcc_afe[u]->data_high = (s >> 8) | k; \
  p_vdcc_afe[u]->data_low = s; \
  afe_wait_write(u);
#define BYTESET(k, g) \
  p_vdcc_afe[u]->data_high = k; \
  p_vdcc_afe[u]->data_low = g; \
  afe_wait_write(u);

/* SCI1 receive: with the SCSI ID switch at ID 0-4 a line monitor (S, D, E, EW, R); otherwise
   binary packets {length, command, arguments} answered with {0xFF, last index, data}. */
void console_handle_rx(void)
{
  union { unsigned char v; struct Bits b; } status;
  unsigned char byte;
  unsigned char *byte_addr;
  unsigned short *word_addr;
  unsigned short s;
  unsigned short line_now;
  unsigned short saved_word;
  int addr;
  int z12;
  int saved_imask;
  int i;
  int u;

  status.v = *(volatile unsigned char *)&p_sci1->SSR.BYTE;
  if (status.b.b6) p_sci1->SSR.BIT.RDRF = 0;
  if (status.b.b5) p_sci1->SSR.BIT.ORER = 0;
  if (status.b.b4) p_sci1->SSR.BIT.FER = 0;
  if (status.b.b3) p_sci1->SSR.BIT.PER = 0;
  byte = p_sci1->RDR;
  if (byte == 0xff && console_packet_out_index != 0) {
    console_packet_out_index = 0;
    return;
  }
  *(unsigned char *)(console_packet_in + console_packet_in_count) = byte;
  console_packet_in_count++;
  console_rx_byte_count++;
  if (hw_scsi_id_table[((struct port_f_bits *)hw_port_f)->scsi_id_switch] >= 5) {
    console_text_mode = 0;
  } else {
    console_text_mode = 1;
  }
  if (console_text_mode != 0) {
    if (console_store_active == 0) {
      if (byte == 0x1b) {
        console_tx_state.v = 2;
        console_send_string(console_msg_crlf);
        console_line_length.v = 0;
      } else if (!p_sci1->SCR.BIT.TE) {
        if (byte == 13) {
          console_line[console_line_length.v] = 0;
          console_rx_state_table[console_tx_state.v]();
        } else if (byte == 8) {
          if (console_line_length.v != 0) {
            console_line_length.v--;
            console_tx_state_saved = console_tx_state.v;
            console_tx_continue = 0;
            console_tx_state.v = 4;
            console_send_string(console_msg_backspace);
          }
        } else if (console_line_length.v < 16) {
          console_line[console_line_length.v++] = byte;
          p_sci1->SCR.BIT.TEIE = 0;
          while (1) {
            if (!p_sci1->SSR.BIT.TDRE) {
              p_sci1->SCR.BIT.TE = 0;
            } else {
              break;
            }
          }
          p_sci1->SCR.BIT.TE = 1;
          p_sci1->TDR = byte;
          p_sci1->SSR.BIT.TDRE = 0;
          while (1) {
            if (p_sci1->SSR.BIT.TEND) break;
          }
          p_sci1->SCR.BIT.TE = 0;
        }
      }
      return;
    }
    else if (byte == 0x1b) {
      console_store_active = 0;
      console_tx_state.v = 2;
      console_send_string(console_msg_crlf);
      console_line_length.v = 0;
      return;
    }
  }
  if (console_packet_in[0] > console_packet_in_count) {
    return;
  }
  p_sci1->SCR.BIT.RE = 0;
  console_packet_out_len[0] = 4;
  console_packet_out_data_0[0] = 0x4e;
  console_packet_out_data_1[0] = 0;
  console_packet_out_data_2[0] = 0;
  switch (console_packet_in_cmd[0]) {
  case CONSOLE_PACKET_SET_GAINS_FRONT:
    u = 0;
    afe_select_source();
    if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1)) {
      hw_init_afe_port(u, 0);
    } else {
      hw_init_afe_port(u, 1);
    }
    if (!u) {
      if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1)) {
        p_vdcc_afe[u]->ctrl.f.adf_select = 0;
      } else {
        p_vdcc_afe[u]->ctrl.f.adf_select = 1;
      }
    } else {
      p_vdcc_afe[u]->ctrl.f.adf_select = 0;
    }
    REGSET(2, console_packet_in_arg_0[0], AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_GAIN_EVEN))
    REGSET(3, console_packet_in_arg_1[0], AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_GAIN_ODD))
    REGSET(4, console_packet_in_arg_2[0], AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_GAIN_EVEN))
    REGSET(5, console_packet_in_arg_3[0], AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_GAIN_ODD))
    REGSET(6, console_packet_in_arg_4[0], AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_GAIN_EVEN))
    REGSET(7, console_packet_in_arg_5[0], AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_GAIN_ODD))
    break;
  case CONSOLE_PACKET_SET_GAINS_BACK:
    u = 1;
    hw_init_afe_port(u, 1);
    REGSET(2, console_packet_in_arg_0[0], AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_GAIN_EVEN))
    REGSET(3, console_packet_in_arg_1[0], AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_GAIN_ODD))
    REGSET(4, console_packet_in_arg_2[0], AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_GAIN_EVEN))
    REGSET(5, console_packet_in_arg_3[0], AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_GAIN_ODD))
    REGSET(6, console_packet_in_arg_4[0], AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_GAIN_EVEN))
    REGSET(7, console_packet_in_arg_5[0], AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_GAIN_ODD))
    break;
  case CONSOLE_PACKET_SET_OFFSETS_FRONT:
    u = 0;
    afe_select_source();
    if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1)) {
      hw_init_afe_port(u, 0);
    } else {
      hw_init_afe_port(u, 1);
    }
    if (!u) {
      if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1)) {
        p_vdcc_afe[u]->ctrl.f.adf_select = 0;
      } else {
        p_vdcc_afe[u]->ctrl.f.adf_select = 1;
      }
    } else {
      p_vdcc_afe[u]->ctrl.f.adf_select = 0;
    }
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_EVEN), console_packet_in_arg_0[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_ODD), console_packet_in_arg_1[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_EVEN), console_packet_in_arg_2[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_ODD), console_packet_in_arg_3[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_EVEN), console_packet_in_arg_4[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_ODD), console_packet_in_arg_5[0])
    break;
  case CONSOLE_PACKET_SET_OFFSETS_BACK:
    u = 1;
    hw_init_afe_port(u, 1);
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_EVEN), console_packet_in_arg_0[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_ODD), console_packet_in_arg_1[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_EVEN), console_packet_in_arg_2[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_ODD), console_packet_in_arg_3[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_EVEN), console_packet_in_arg_4[0])
    BYTESET(AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_ODD), console_packet_in_arg_5[0])
    break;
  case CONSOLE_PACKET_WRITE_WORD:
    addr = console_packet_in_arg_0[0] << 24;
    addr += console_packet_in_arg_1[0] << 16;
    addr += console_packet_in_arg_2[0] << 8;
    addr += console_packet_in_arg_3[0];
    word_addr = (unsigned short *)addr;
    s = (console_packet_in_arg_4[0] << 8) + console_packet_in_arg_5[0];
    *word_addr = s;
    break;
  case CONSOLE_PACKET_WRITE_BYTE:
    addr = console_packet_in_arg_0[0] << 24;
    addr += console_packet_in_arg_1[0] << 16;
    addr += console_packet_in_arg_2[0] << 8;
    addr += console_packet_in_arg_3[0];
    byte_addr = (unsigned char *)addr;
    *byte_addr = console_packet_in_arg_4[0];
    break;
  case CONSOLE_PACKET_WRITE_WORD_LINES:
    addr = console_packet_in_arg_0[0] << 24;
    addr += console_packet_in_arg_1[0] << 16;
    addr += console_packet_in_arg_2[0] << 8;
    addr += console_packet_in_arg_3[0];
    word_addr = (unsigned short *)addr;
    saved_imask = _builtin_get_imask();
    s = line_side_state[0].line_count;
    _builtin_set_imask(11);
    do {
      line_now = line_side_state[0].line_count;
    } while (s == line_now);
    _builtin_set_imask(15);
    s = (console_packet_in_arg_4[0] << 8) + console_packet_in_arg_5[0];
    saved_word = *word_addr;
    *word_addr = s;
    _builtin_set_imask(11);
    for (i = 0; i < console_packet_in_arg_6[0]; i++) {
      s = line_side_state[0].line_count;
      do {
        line_now = line_side_state[0].line_count;
      } while (s == line_now);
    }
    _builtin_set_imask(15);
    *word_addr = saved_word;
    _builtin_set_imask(saved_imask);
    break;
  case 0x21:
    break;
  case 0x30:
    break;
  case 0x40:
    break;
  case 0x50:
    break;
  case 0x80:
    break;
  case 0x81:
    break;
  case CONSOLE_PACKET_POWER_SAVE:
    power_state.f.enter_request = 1;
    break;
  case CONSOLE_PACKET_WAKE:
    power_state.f.leave_request = 1;
    break;
  case 0x84:
    break;
  case 0x86:
    break;
  case CONSOLE_PACKET_READ_WORDS:
    addr = console_packet_in_arg_0[0] << 24;
    addr += console_packet_in_arg_1[0] << 16;
    addr += console_packet_in_arg_2[0] << 8;
    addr += console_packet_in_arg_3[0];
    word_addr = (unsigned short *)addr;
    console_packet_out_len[0] = 9;
    s = *word_addr++;
    console_packet_out_data_0[0] = s >> 8;
    console_packet_out_data_1[0] = s;
    s = *word_addr++;
    console_packet_out_data_2[0] = s >> 8;
    console_packet_out_data_3[0] = s;
    s = *word_addr++;
    console_packet_out_data_4[0] = s >> 8;
    console_packet_out_data_5[0] = s;
    s = *word_addr++;
    console_packet_out_data_6[0] = s >> 8;
    console_packet_out_data_7[0] = s;
    break;
  case CONSOLE_PACKET_READ_BYTES:
    addr = console_packet_in_arg_0[0] << 24;
    addr += console_packet_in_arg_1[0] << 16;
    addr += console_packet_in_arg_2[0] << 8;
    addr += console_packet_in_arg_3[0];
    byte_addr = (unsigned char *)addr;
    console_packet_out_len[0] = 9;
    console_packet_out_data_0[0] = *byte_addr++;
    console_packet_out_data_1[0] = *byte_addr++;
    console_packet_out_data_2[0] = *byte_addr++;
    console_packet_out_data_3[0] = *byte_addr++;
    console_packet_out_data_4[0] = *byte_addr++;
    console_packet_out_data_5[0] = *byte_addr++;
    console_packet_out_data_6[0] = *byte_addr++;
    console_packet_out_data_7[0] = *byte_addr;
    break;
  case 0xa0:
    break;
  case CONSOLE_PACKET_HOOK:
    console_hook_rx_byte((char)console_packet_in_arg_0[0]);
    break;
  case CONSOLE_PACKET_READ_REVISION:
    console_packet_out_len[0] = 5;
    util_mem_copy(console_packet_out_data_0, scsi_revision, 4);
    break;
  default:
    break;
  }
  console_packet_in_count = 0;
  console_packet_out_index = 1;
  console_packet_out[0] = 0xff;
  while (1) {
    if (!p_sci1->SSR.BIT.TDRE) {
      p_sci1->SCR.BIT.TE = 0;
    } else {
      break;
    }
  }
  p_sci1->SCR.BIT.TE = 1;
  p_sci1->TDR = console_packet_out[0];
  p_sci1->SSR.BIT.TDRE = 0;
  p_sci1->SCR.BIT.TEIE = 1;
}

void console_send_next(void)
{
  console_send_byte();
}

void console_send_byte(void)
{
  if (console_text_mode != 0) {
    console_flush_tx();
    return;
  }
  if (console_packet_out_index <= console_packet_out_len[0]) {
    while (1) {
      if (!p_sci1->SSR.BIT.TDRE) {
        p_sci1->SCR.BIT.TE = 0;
      } else {
        break;
      }
    }
    console_clear_rx_errors();
    p_sci1->SCR.BIT.TE = 1;
    p_sci1->TDR = console_packet_out[console_packet_out_index];
    console_packet_out_index++;
    p_sci1->SSR.BIT.TDRE = 0;
    p_sci1->SCR.BIT.TEIE = 1;
    return;
  }
  while (1) {
    if (p_sci1->SSR.BIT.TEND) break;
  }
  p_sci1->SCR.BIT.TE = 0;
  p_sci1->SCR.BIT.RE = 1;
  p_sci1->SCR.BIT.RIE = 1;
  return;
}

void console_clear_rx_errors(void)
{
  union { unsigned char v; struct Bits b; } status;
  unsigned char t;

  status.v = *(volatile unsigned char *)&p_sci1->SSR.BYTE;
  if (status.b.b5) p_sci1->SSR.BIT.ORER = 0;
  if (status.b.b4) p_sci1->SSR.BIT.FER = 0;
  if (status.b.b3) p_sci1->SSR.BIT.PER = 0;
  if (status.b.b6) {
    t = p_sci1->RDR;
    p_sci1->SSR.BIT.RDRF = 0;
  }
}

void console_hook_rx_byte(char byte)
{
}

void console_rx_state_0(void)
{
  console_send_crlf();
}

void console_rx_state_idle(void)
{
}

void console_rx_state_5(void)
{
  struct console_hex_state hex;
  struct console_scan_state scan;

  hex.digits = 4;
  hex.p = console_line;
  console_parse_hex(&hex);
  scan.separator = 0x20;
  scan.column = hex.digits;
  scan.p = console_line;
  console_skip_char(&scan);
  if (!*scan.p) {
    if (hex.digits) {
      console_segment = hex.v;
    }
    console_tx_state.v = 2;
    console_send_string(console_msg_crlf);
    console_line_length.v = 0;
  } else {
    console_tx_state.v = 2;
    console_send_string(console_msg_crlf_illegal_crlf);
    console_line_length.v = 0;
  }
}

void console_rx_state_6(void)
{
}

void console_rx_state_7(void)
{
  struct console_hex_state hex;
  struct console_scan_state scan;
  unsigned long addr;

  hex.digits = 2;
  hex.p = console_line;
  console_parse_hex(&hex);
  scan.separator = 0x20;
  scan.column = hex.digits;
  scan.p = console_line;
  console_skip_char(&scan);
  if (!*scan.p) {
    if (hex.digits != 0) {
      addr = ((*(volatile unsigned long *)&console_segment) << 16) + console_offset;
      if (addr >= 0x40000) {
        *(unsigned char *)addr = hex.v;
      }
    }
    console_offset++;
    console_print_byte_at();
    console_line_length.v = 0;
    console_flush_tx();
  } else {
    console_tx_state_saved = console_tx_state.v;
    console_tx_continue = 1;
    console_tx_state.v = 4;
    console_send_string(console_msg_crlf_illegal);
    console_line_length.v = 0;
  }
}

void console_print_byte_at(void)
{
  unsigned long addr;
  unsigned char *p;

  p = console_tx_line;
  *p = 13;
  p[1] = 10;
  p[2] = 32;
  p += 3;
  console_format_hex16(console_segment, &p);
  *p++ = ':';
  console_format_hex16(console_offset, &p);
  *p++ = ' ';
  addr = (console_segment << 16) + console_offset;
  console_format_hex8(*(unsigned char *)addr, &p);
  *p++ = ' ';
  *p++ = '-';
  *p++ = ' ';
  console_tx_count = 0x12;
  console_tx_index = 0;
  console_tx_continue = 0;
  console_tx_segment = 0;
  console_tx_ptr.v = (unsigned long)console_tx_line;
  p_sci1->SCR.BIT.TE = 1;
  p_sci1->SCR.BIT.TEIE = 1;
}

void console_rx_state_8(void)
{
  struct console_hex_state hex;
  struct console_scan_state scan;
  unsigned long addr;

  hex.digits = 4;
  hex.p = console_line;
  console_parse_hex(&hex);
  scan.separator = 0x20;
  scan.column = hex.digits;
  scan.p = console_line;
  console_skip_char(&scan);
  if (!*scan.p) {
    if (hex.digits != 0) {
      addr = ((*(volatile unsigned long *)&console_segment) << 16) + console_offset;
      if (addr >= 0x40000) {
        *(unsigned short *)addr = hex.v;
      }
    }
    console_offset += 2;
    console_print_word_at();
    console_line_length.v = 0;
    console_flush_tx();
  } else {
    console_tx_state_saved = console_tx_state.v;
    console_tx_continue = 1;
    console_tx_state.v = 4;
    console_send_string(console_msg_crlf_illegal);
    console_line_length.v = 0;
  }
}

void console_print_word_at(void)
{
  unsigned long addr;
  unsigned char *p;

  p = console_tx_line;
  *p++ = 13;
  *p++ = 10;
  *p++ = 32;
  console_format_hex16(console_segment, &p);
  *p++ = ':';
  console_format_hex16(console_offset, &p);
  *p++ = ' ';
  addr = (console_segment << 16) + console_offset;
  console_format_hex16(*(unsigned short *)addr, &p);
  *p++ = ' ';
  *p++ = '-';
  *p++ = ' ';
  console_tx_count = 0x14;
  console_tx_index = 0;
  console_tx_continue = 0;
  console_tx_segment = 0;
  console_tx_ptr.v = (unsigned long)console_tx_line;
  p_sci1->SCR.BIT.TE = 1;
  p_sci1->SCR.BIT.TEIE = 1;
}

void console_rx_state_9(void)
{
  struct console_hex_state hex;
  struct console_scan_state scan;
  unsigned long p;

  hex.digits = 2;
  hex.p = console_line;
  console_parse_hex(&hex);
  scan.separator = 0x20;
  scan.column = hex.digits;
  scan.p = console_line;
  console_skip_char(&scan);
  if (!*scan.p && hex.digits) {
    console_store_value = hex.v;
    console_store_active = 1;
    console_line_length.v = 0;
    console_tx_state_saved = console_tx_state.v;
    console_tx_continue = 0;
    console_tx_state.v = 4;
    console_send_string(console_msg_esc_to_exit);
    console_line_length.v = 0;
  } else {
    console_tx_state.v = 2;
    console_send_string(console_msg_crlf_illegal_crlf);
    console_line_length.v = 0;
  }
}

void console_unused_flush_tx(void)
{
  console_flush_tx();
}

void console_tx_if_done(void)
{
  if (!p_sci1->SSR.BIT.TEND) {
    return;
  }
  console_flush_tx();
}

void console_flush_tx(void)
{
  unsigned char *p;

top:
  if (console_tx_count) {
    p = (unsigned char *)((console_tx_segment << 16) + console_tx_ptr.v + console_tx_index++);
    p_sci1->SCR.BIT.TEIE = 0;
    while (1) {
      if (!p_sci1->SSR.BIT.TDRE) {
        p_sci1->SCR.BIT.TE = 0;
      } else {
        break;
      }
    }
    p_sci1->SCR.BIT.TE = 1;
    p_sci1->TDR = *p;
    p_sci1->SSR.BIT.TDRE = 0;
    while (1) {
      if (p_sci1->SSR.BIT.TEND) break;
    }
    if (*(volatile unsigned char *)&console_tx_index != console_tx_count) goto top;
  }
  console_tx_count = 0;
  console_tx_index = 0;
  console_tx_state_table[console_tx_state.v]();
  if (console_tx_count != 0) goto top;
}

void console_reset_tx(void)
{
  console_tx_state.v = 0;
  console_tx_state_saved = 0;
  console_tx_ptr.v = 0;
  console_tx_spare = 0;
  console_tx_count = 0;
  console_tx_index = 0;
  console_tx_state.v = 0;
  p_sci1->SCR.BIT.TE = 0;
  p_sci1->SCR.BIT.TEIE = 0;
}

void console_tx_state_idle(void)
{
}

void console_tx_state_1(void)
{
  console_reset_tx();
}

void console_tx_state_2(void)
{
  console_tx_state.v = 1;
  console_send_string(console_msg_prompt);
}

void console_tx_state_3(void)
{
  console_run_command();
}

void console_tx_state_4(void)
{
  if (console_tx_continue != 1) {
    p_sci1->SCR.BIT.TE = 0;
    p_sci1->SCR.BIT.TEIE = 0;
  }
  console_tx_index = 0;
  console_tx_count = 0;
  console_tx_state.v = console_tx_state_saved;
  console_tx_state_table[console_tx_state.v]();
}

void console_tx_state_5(void)
{
  p_sci1->SCR.BIT.TE = 0;
  p_sci1->SCR.BIT.TEIE = 0;
  console_line_length.v = 0;
  console_tx_index = 0;
}

void console_tx_state_6(void)
{
  unsigned char value;
  unsigned char ascii;
  unsigned char i;
  unsigned char *src;
  unsigned char *ascii_dst;
  unsigned char *p;

  p = console_tx_line;
  for (i = 77; i > 0; i--) {
    *p++ = ' ';
  }
  p = console_tx_line;
  console_format_hex16(console_segment, &p);
  *p++ = ':';
  console_format_hex16(console_offset, &p);
  *p++ = ' ';
  *p++ = ' ';
  src = (unsigned char *)((console_segment << 16) + console_offset);
  ascii_dst = console_tx_line_ascii;
  for (i = 16; i > 0; i--) {
    value = ascii = *src++;
    if (ascii < 32 || ascii >= 128) {
      ascii = '.';
    }
    *ascii_dst++ = ascii;
    console_format_hex8(value, &p);
    if (console_offset == console_dump_end.v) {
      console_tx_state.v = 2;
      console_line_length.v = 0;
      break;
    }
    value = ' ';
    if (i == 9) {
      value = '-';
    }
    *p++ = value;
    console_offset++;
  }
  p = console_tx_line_crlf;
  *p++ = 13;
  *p = 10;
  console_tx_count = 80;
  console_tx_segment = 0;
  console_tx_ptr.v = (unsigned long)console_tx_line;
  console_tx_index = 0;
}

void console_tx_state_7(void)
{
  if (console_tx_continue == 1) {
    console_print_byte_at();
  } else {
    p_sci1->SCR.BIT.TE = 0;
    p_sci1->SCR.BIT.TEIE = 0;
  }
  console_line_length.v = 0;
  console_tx_index = 0;
}

void console_tx_state_8(void)
{
  if (console_tx_continue == 1) {
    console_print_word_at();
  } else {
    p_sci1->SCR.BIT.TE = 0;
    p_sci1->SCR.BIT.TEIE = 0;
  }
  console_line_length.v = 0;
  console_tx_index = 0;
}

void console_tx_state_9(void)
{
  p_sci1->SCR.BIT.TE = 0;
  p_sci1->SCR.BIT.TEIE = 0;
  console_line_length.v = 0;
  console_tx_index = 0;
}

void console_send_crlf(void)
{
  console_tx_state.v = 3;
  console_send_string(console_msg_crlf);
}

void console_run_command(void)
{
  struct console_scan_state scan;
  unsigned char j;
  struct console_cmd *entry;
  struct console_cmd **slot;
  unsigned char *word_start;
  unsigned char ch;

  scan.separator = 32;
  scan.column = 0;
  scan.p = console_line;
  console_skip_char(&scan);
  if (*scan.p) {
    slot = console_cmd_table;
    word_start = scan.p;
    for (;; slot++) {
      if ((entry = *slot) == 0) {
        console_reply_illegal();
        break;
      }
      j = 0;
      scan.p = word_start;
      while (1) {
        if (entry->name[j] == 0) {
          ch = *scan.p;
          if (ch != 0 && ch != ' ') {
            if (ch == 'W' || ch == 'w') break;
inc:
            scan.p++;
            j++;
            continue;
          }
          scan.column += j;
          entry = *slot;
          console_tx_state.v = entry->id;
          entry->handler(scan.column);
          return;
        } else {
          ch = *scan.p;
          if (ch >= 'a' && ch <= 'z') {
            ch -= 32;
          }
          if (ch == entry->name[j]) goto inc;
          break;
        }
      }
    }
  } else {
    console_send_prompt();
  }
}

void console_reply_illegal(void)
{
  console_tx_state.v = 2;
  console_send_string(console_msg_illegal);
  console_line_length.v = 0;
}

void console_send_prompt(void)
{
  console_tx_state.v = 1;
  console_send_string(console_msg_prompt);
  console_line_length.v = 0;
}

void console_cmd_s(unsigned char pos)
{
  struct console_scan_state scan;
  unsigned char *p;

  scan.separator = 32;
  scan.column = pos;
  scan.p = console_line;
  console_skip_char(&scan);
  if (!*scan.p) {
    p = console_tx_line;
    *p++ = ' ';
    console_format_hex16(console_segment, &p);
    *p = '-';
    console_tx_segment = 0;
    console_tx_ptr.v = (unsigned long)console_tx_line;
    console_tx_count = 6;
    console_tx_index = 0;
    return;
  }
  console_reply_illegal();
}

void console_cmd_d(unsigned char pos)
{
  struct console_scan_state scan;
  struct console_hex_state hex;
  unsigned char ch;

  scan.separator = 32;
  scan.column = pos;
  scan.p = console_line;
  if (!console_skip_char(&scan)) {
    hex.digits = 4;
    hex.p = scan.p;
    console_parse_hex(&hex);
    scan.column += hex.digits;
    console_offset = hex.v;
    ch = *hex.p;
    if (ch == '-') {
      hex.p++;
      hex.digits = 4;
      console_parse_hex(&hex);
      scan.column += hex.digits;
      console_dump_end.v = hex.v;
    } else {
      if (ch != '&') goto err;
      hex.p++;
      hex.digits = 2;
      console_parse_hex(&hex);
      scan.column += hex.digits;
      console_dump_end.v = console_offset + hex.v;
    }
    scan.separator = 32;
    scan.column++;
    scan.p = console_line;
    console_skip_char(&scan);
    if (*scan.p) goto err;
    console_tx_index = 0;
    console_tx_state_6();
    return;
  }
err:
  console_reply_illegal();
}

/* Console addresses are console_segment << 16 + a 4-digit hex offset ("S" sets console_segment). */
void console_cmd_e(unsigned char pos)
{
  struct console_scan_state scan;
  struct console_hex_state hex;
  unsigned char ch;
  unsigned char *out;
  unsigned char *src;

  scan.separator = 32;
  scan.column = pos;
  scan.p = console_line;
  if (!console_skip_char(&scan)) {
    hex.digits = 4;
    hex.p = scan.p;
    console_parse_hex(&hex);
    scan.column += hex.digits;
    scan.separator = 32;
    scan.p = console_line;
    console_skip_char(&scan);
    if (!*scan.p && hex.digits) {
      console_offset = hex.v;
      out = console_tx_line;
      *out++ = ' ';
      console_format_hex16(console_segment, &out);
      *out++ = ':';
      console_format_hex16(console_offset, &out);
      *out++ = ' ';
      src = (unsigned char *)((console_segment << 16) + console_offset);
      ch = *src;
      console_format_hex8(ch, &out);
      *out++ = ' ';
      *out++ = '-';
      *out++ = ' ';
      console_tx_segment = 0;
      console_tx_ptr.v = (unsigned long)console_tx_line;
      console_tx_count = 16;
      console_tx_index = 0;
      console_tx_continue = 0;
      return;
    }
  }
  console_reply_illegal();
}

void console_cmd_ew(unsigned char pos)
{
  struct console_scan_state scan;
  unsigned long dummy;
  struct console_hex_state hex;
  unsigned long word;
  unsigned char *out;
  unsigned short *src;

  scan.separator = 32;
  scan.column = pos;
  scan.p = console_line;
  if (!console_skip_char(&scan)) {
    hex.digits = 4;
    hex.p = scan.p;
    console_parse_hex(&hex);
    scan.column += hex.digits;
    scan.separator = 32;
    scan.p = console_line;
    console_skip_char(&scan);
    if (!*scan.p && hex.digits) {
      console_offset = hex.v;
      console_offset &= 0xfffffffe;
      out = console_tx_line;
      *out++ = ' ';
      console_format_hex16(console_segment, &out);
      *out++ = ':';
      console_format_hex16(console_offset, &out);
      *out++ = ' ';
      src = (unsigned short *)((console_segment << 16) + console_offset);
      word = *src;
      console_format_hex16(word, &out);
      *out++ = ' ';
      *out++ = '-';
      *out++ = ' ';
      console_tx_segment = 0;
      console_tx_ptr.v = (unsigned long)console_tx_line;
      console_tx_count = 18;
      console_tx_index = 0;
      console_tx_continue = 0;
      return;
    }
  }
  console_reply_illegal();
}

void console_cmd_r(unsigned char pos)
{
  struct console_scan_state scan;
  struct console_hex_state hex;
  unsigned char ch;
  unsigned char *out;
  unsigned char *src;

  scan.separator = 32;
  scan.column = pos;
  scan.p = console_line;
  if (!console_skip_char(&scan)) {
    hex.digits = 4;
    hex.p = scan.p;
    console_parse_hex(&hex);
    scan.column += hex.digits;
    scan.separator = 32;
    scan.p = console_line;
    console_skip_char(&scan);
    if (!*scan.p && hex.digits) {
      console_offset = hex.v;
      out = console_tx_line;
      *out++ = ' ';
      console_format_hex16(console_segment, &out);
      *out++ = ':';
      console_format_hex16(console_offset, &out);
      *out++ = ' ';
      src = (unsigned char *)((console_segment << 16) + console_offset);
      ch = *src;
      console_format_hex8(ch, &out);
      *out++ = ' ';
      *out++ = '-';
      *out++ = ' ';
      console_tx_segment = 0;
      console_tx_ptr.v = (unsigned long)console_tx_line;
      console_tx_count = 16;
      console_tx_index = 0;
      console_tx_continue = 0;
      return;
    }
  }
  console_reply_illegal();
}

void console_send_string(unsigned char *p)
{
  console_tx_count = *p++;
  console_tx_segment = ((unsigned long)p >> 16) & 0xf000;
  console_tx_ptr.v = (unsigned long)p;
  console_tx_index = 0;
  while (1) {
    if (!p_sci1->SSR.BIT.TDRE) {
      p_sci1->SCR.BIT.TE = 0;
    } else {
      break;
    }
  }
  p_sci1->SCR.BIT.TE = 1;
  console_flush_tx();
}

unsigned char console_skip_char(struct console_scan_state *scan)
{
  if (scan->column > 14) {
    return 1;
  }
  scan->p += scan->column;
  while (1) {
    if (scan->column > 15) {
      return 1;
    }
    if (*scan->p != scan->separator) {
      return 0;
    }
    if (!*scan->p) {
      return 0x80;
    }
    scan->p++;
    scan->column++;
  }
}

unsigned char console_parse_hex(struct console_hex_state *hex)
{
  unsigned char digit;
  unsigned char i;
  unsigned char result;

  result = 0;
  hex->v = 0;
  for (i = 0; i < hex->digits; i++) {
    digit = console_hex_digit_value(*hex->p);
    if (digit & 0x80) {
      result |= 0x80;
      break;
    } else {
      hex->v <<= 4;
      hex->v |= digit;
      hex->p++;
    }
  }
  hex->digits = i;
  return result;
}

unsigned char console_hex_digit_value(unsigned char ch)
{
  if (ch >= '0' && ch <= '9') {
    return ch & 15;
  }
  if (ch > 'F') {
    ch -= 32;
  }
  if (ch >= 'A' && ch <= 'F') {
    ch &= 15;
    ch += 9;
    return ch;
  }
  return ch | 0x80;
}

void console_format_hex16(unsigned long value, unsigned char **out)
{
  console_format_hex8(value >> 8, out);
  console_format_hex8(value, out);
}

void console_format_hex8(unsigned char value, unsigned char **out)
{
  unsigned char digit;

  digit = (value >> 4) | 0x30;
  if (digit > 0x39) {
    digit = digit + 7;
  }
  **out = digit;
  (*out)++;
  digit = (value & 0xf) | 0x30;
  if (digit > 0x39) {
    digit = digit + 7;
  }
  **out = digit;
  (*out)++;
}

void console_store_byte(void)
{
  unsigned long addr;

  if (console_store_active != 0) {
    addr = ((*(volatile unsigned long *)&console_segment) << 16) + console_offset;
    if (addr >= 0x40000) { /* never into the ROM */
      *(unsigned char *)addr = console_store_value;
    }
    if (*(unsigned char *)addr) {
    }
  }
}
