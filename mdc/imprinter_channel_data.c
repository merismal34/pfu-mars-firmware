#include "mdc.h"

struct imprinter_channel_state imprinter_state;
unsigned short imprinter_filler_1;
struct imprinter_channel_state *imprinter_channel;
struct imprinter_header imprinter_entry_header;
unsigned char imprinter_entry_text[41];
unsigned char imprinter_filler_2;
union imprinter_status_flags_bits imprinter_status_flags;
