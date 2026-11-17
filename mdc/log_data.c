#include "mdc.h"

unsigned short log_frozen;
unsigned short log_limit;
unsigned char log_filler_1[13];
union imprinter_line_status_bits imprinter_line_status;
unsigned char log_filler_2[8];
