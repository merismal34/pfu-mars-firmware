#include "mdc.h"

void hw_hook_p3(void)
{
    unsigned char pending;

    pending = 1;
    while (pending) {
        pending = 0;
    }
}
