/* xfer.h: declarations of xfer.c, xfer_length.c, xfer_wait.c, xfer_reselect.c, xfer_reconnect.c,
   xfer_dma.c. */
#ifndef XFER_H
#define XFER_H
#include "common.h"

/* xfer_state_front / xfer_state_back: the index into xfer_state_table (a handler returns the next state; 0 idle) */
#define XFER_STATE_INIT           1
#define XFER_STATE_COMPUTE_LENGTH 2
#define XFER_STATE_WAIT_DATA      3
#define XFER_STATE_START_DMA      9

/* functions */
extern int xfer_init(void);
extern int xfer_compute_length(void);
extern int xfer_wait_data(void);
extern int xfer_state_4(void);
extern int xfer_state_5(void);
extern int xfer_state_6(void);
extern int xfer_state_7(void);
extern int xfer_state_8(void);
extern int xfer_start_dma(void);
extern int xfer_state_10(void);
extern int xfer_state_11(void);
extern int xfer_state_12(void);

#endif
