/* fw.h: the shared declarations of the application. Every source file of the application includes
   it; the boot programs (boot.h) do not. It declares every symbol through the headers it includes:
   common.h (shared types), one header per module and globals.h (the variables of the globals files).
   A file that needs another view of a symbol hides the header's: `#define name name_hdr` before the
   include, `#undef name` after it, then its own declaration.
   The compiler lays out section B in the order of each object's first declaration, so globals.h
   declares the variables in their definition order and with their types. */
#ifndef FW_H
#define FW_H

#include "common.h"
#include "peripherals.h"
#include "start.h"
#include "rt.h"
#include "edge.h"
#include "button.h"
#include "img.h"
#include "scsi.h"
#include "xfer.h"
#include "dirt.h"
#include "hw_selftest.h"
#include "line_calib.h"
#include "page.h"
#include "tick.h"
#include "log.h"
#include "isr.h"
#include "uart_link.h"
#include "link.h"
#include "util.h"
#include "iic.h"
#include "afe_usb.h"
#include "usb.h"
#include "panel.h"
#include "err.h"
#include "job.h"
#include "maint.h"
#include "testmode.h"
#include "selftest_mark.h"
#include "jpeg.h"
#include "mdc.h"
#include "power.h"
#include "hook.h"
#include "main.h"
#include "calib_task.h"
#include "console.h"
#include "flash.h"
#include "globals.h"

#endif
