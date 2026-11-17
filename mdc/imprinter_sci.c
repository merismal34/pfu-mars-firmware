#include "mdc.h"

void imprinter_sci0_rx(void)
{
    imprinter_sci0_receive();
}

void imprinter_sci0_tx(void)
{
    p_sci0->SCR.BIT.TIE = 0;
    imprinter_sci0_transmit();
}

void imprinter_sci0_tx_end(void)
{
    p_sci0->SCR.BIT.TEIE = 0;
    if (!p_sci0->SSR.BIT.TEND)
        return;
    imprinter_sci0_transmit();
}

void imprinter_sci1_rx(void)
{
    imprinter_sci1_receive();
}

void imprinter_sci1_tx(void)
{
    p_sci1->SCR.BIT.TIE = 0;
    imprinter_sci1_transmit();
}

void imprinter_sci1_tx_end(void)
{
    p_sci1->SCR.BIT.TEIE = 0;
    if (!p_sci1->SSR.BIT.TEND)
        return;
    imprinter_sci1_transmit();
}
