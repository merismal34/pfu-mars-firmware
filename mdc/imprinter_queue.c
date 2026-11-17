#include "mdc.h"

void imprinter_queue_run(struct imprinter_queue_state *queue, unsigned char ch)
{
    switch (queue->state) {
    case 1:
        queue->state = 9;
        break;
    case 2:
        if (queue->write_index != queue->read_index) {
            imprinter_queue_load_counter(queue, ch);
            if (queue->b252 & 0x80) {
                imprinter_send_wait(1, 0x30, ch);
                if (!imprinter_tx_data.view.flags.bit.b7) {
                    queue->state = 3;
                }
            } else {
                if (queue->entries[queue->read_index & 3].send_flags & 0x10) {
                    util_mem_copy(imprinter_entry_header.flags, queue->entries[queue->read_index & 3].header.flags, 0x10);
                    imprinter_send_wait(1, 0xc, ch);
                    queue->state = 4;
                } else {
                    queue->state = 5;
                }
            }
        }
        break;
    case 3:
        util_mem_copy(imprinter_entry_header.flags, queue->entries[queue->read_index & 3].header.flags, 0x10);
        imprinter_send_wait(1, 0xc, ch);
        queue->entries[queue->read_index & 3].send_flags &= 0xdf;
        if (!imprinter_tx_data.view.flags.bit.b7) {
            queue->state = 4;
        }
        break;
    case 4:
        if (imprinter_channel->busy == 0) {
            if (imprinter_queue_check_error(&queue->err, ch)) {
                break;
            }
            queue->state = 5;
        } else {
            break;
        }
    case 5:
        if (queue->entries[queue->read_index & 3].send_flags & 0x20) {
            util_mem_copy(imprinter_entry_text, &queue->entries[queue->read_index & 3].text_len, 0x29);
            imprinter_send_wait(1, 10, ch);
            if (!imprinter_tx_data.view.flags.bit.b7) {
                queue->state = 6;
            }
            break;
        } else {
            queue->state = 6;
        }
    case 6:
        if (imprinter_channel->busy == 0) {
            if (imprinter_queue_check_error(&queue->err, ch)) {
                break;
            }
            queue->b252 |= 1;
            imprinter_send_wait(1, 0xe, ch);
            if (!imprinter_tx_data.view.flags.bit.b7) {
                imprinter_queue_advance_counter(queue, ch);
                queue->state = 7;
            }
        }
        break;
    case 7:
        if (queue->b252 & 2) {
            imprinter_send(0x10, ch);
            queue->s250 = (queue->b252 & 0x80) ? 0xffff : queue->entries[queue->read_index & 3].text_len * imprinter_queue_time_factor / 10 + queue->entries[queue->read_index & 3].header.y_offset;
            if (!imprinter_tx_data.view.flags.bit.b7) {
                queue->read_index++;
                queue->state = 8;
            }
        }
        break;
    case 8:
        if (imprinter_channel->busy == 0) {
            if (imprinter_queue_check_error(&queue->err, ch)) {
                break;
            }
            queue->state = 9;
        } else {
            break;
        }
    case 9:
        queue->l246 = 900000;
        queue->state = 10;
    case 10:
        if (queue->b252 & 8) {
            queue->b252 &= 0xfd;
            queue->b252 &= 0xf7;
            if (queue->s250) {
                queue->s250 = 0;
                queue->b252 &= 0xfe;
                imprinter_send((mailbox.settings_5f & 0x20) ? 0x13 : 0x12, ch);
                queue->l246 = 900000;
                queue->state = 11;
            } else {
                queue->state = 2;
            }
        } else {
            if (feed_pick_edge_count == 0 && queue->l246 == 0) {
                queue->state = 13;
            }
        }
        break;
    case 11:
        if (!imprinter_tx_data.view.flags.bit.b7) {
            imprinter_send((mailbox.settings_5f & 0x20) ? 0x13 : 0x12, ch);
        } else {
            queue->l246 = 900000;
            queue->state = 12;
        }
        break;
    case 12:
        if (imprinter_channel->busy == 0) {
            imprinter_queue_check_error(&queue->err, ch);
            queue->state = 2;
        }
        break;
    }
    if (queue->err.code) {
        err_flags.b = queue->err.flags.b;
        err_code = queue->err.code;
        queue->state = 0xff;
    }
}

unsigned char imprinter_queue_check_error(struct imprinter_error *error, unsigned char ch)
{
    if (imprinter_state.w24.w) {
        if (imprinter_state.w24.w & 0xf080) {
            error->flags.bit.device = 1;
            error->code = 0xb2;
        } else if ((imprinter_state.w24.w & 0x400)) {
            error->flags.bit.device = 1;
            error->code = 0xb3;
        } else if ((imprinter_state.w24.w & 0x100)) {
            error->flags.bit.device = 1;
            error->code = 0xb5;
        } else if (imprinter_state.w24.w & 0x20) {
            mailbox.multifeed_flags.bit.imprinter = 1;
            mailbox.multifeed_flags_imprinter |= mailbox.multifeed_flags.b;
        } else if (imprinter_state.w24.w & 0x10) {
            error->flags.bit.document = 1;
            error->code = 0xb4;
        } else if (imprinter_state.w24.w & 8) {
            error->flags.bit.device = 1;
            error->code = 0xb8;
        } else if (imprinter_state.w24.w & 2) {
            error->flags.bit.device = 1;
            error->code = 0xb6;
        } else if (imprinter_state.w24.w & 1) {
            error->flags.bit.device = 1;
            error->code = 0xb7;
        }
    }
    return error->code;
}

void imprinter_queue_load_counter(struct imprinter_queue_state *queue, unsigned char ch)
{
    if (queue->entries[queue->read_index & 3].header.flags[1] & 0x40) {
        queue->counter.value = queue->entries[queue->read_index & 3].header.counter;
        queue->entries[queue->read_index & 3].send_flags |= 0x30;
    }
}

void imprinter_queue_advance_counter(struct imprinter_queue_state *queue, unsigned char ch)
{
    queue->counter.value += queue->entries[queue->read_index & 3].counter_increment;
    if (queue->entries[queue->read_index & 3].header.flags[1] & 2) {
        queue->counter.top_byte = 0;
    } else {
        queue->counter.top_word = 0;
    }
}

void imprinter_queue_undo_counter(struct imprinter_queue_state *queue, unsigned char ch)
{
    queue->counter.value -= queue->entries[(queue->read_index - 1) & 3].counter_increment;
    if (queue->entries[(queue->read_index - 1) & 3].header.flags[1] & 2) {
        queue->counter.top_byte = 0;
    } else {
        queue->counter.top_word = 0;
    }
}

void imprinter_queue_fill_entry(struct imprinter_queue_entry *entry, struct imprinter_queue_cmd *cmd)
{
    unsigned char flags;
    unsigned char byte;

    flags = 0;
    entry->send_flags = cmd->send_flags;
    entry->header.flags[0] = 0x80;
    if (cmd->b3 & 1) {
        entry->header.flags[0] |= 2;
    }
    if (cmd->b1 & 0x10) {
        entry->header.flags[0] |= 0x20;
    }
    if (cmd->b5 & 0x80) {
        flags |= 0x80;
    }
    if (cmd->b3 & 0x10) {
        flags |= 0x40;
    }
    if (cmd->b3 & 0x40) {
        flags |= 2;
    }
    flags |= cmd->b5 & 0x30;
    entry->header.flags[1] = flags;
    entry->header.s4 = 0x85;
    entry->header.y_offset = (cmd->y_offset_low | (unsigned short)(cmd->y_offset_high << 8)) / 3;
    entry->header.l8 = 0x2aefa;
    entry->header.counter = (cmd->counter_low | (unsigned short)(cmd->counter_mid << 8)) | ((unsigned long)(cmd->counter_high + IMPRINTER_QUEUE_HIGH_OFFSET) << 16);
    entry->counter_increment = entry->header.counter_step = cmd->counter_step;
    if ((cmd->b5 & 0x30) == 0x30) {
        entry->counter_increment = -entry->counter_increment;
    }
    entry->header.b17 = 0x1a;
    flags = 0;
    do {
        if ((byte = entry->text[flags] = cmd->text[flags]) == 0) {
            flags++;
            break;
        } else if (byte == 0xff) {
            entry->text[flags] = 0;
            flags += 5;
            break;
        } else if (byte == 0xfc) {
            entry->header.flags[1] |= 1;
            entry->text[flags] = 0;
            flags += 8;
            break;
        }
    } while (++flags < 0x28);
    entry->text_len = flags;
}
