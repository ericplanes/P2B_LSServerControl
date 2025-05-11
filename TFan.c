#include "TFan.h"
#include <xc.h>

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

void FAN_Init(void)
{
    TRISCbits.TRISC1 = 0; // FAN A
    TRISCbits.TRISC2 = 0; // FAN B

    LATCbits.LATC1 = 0;
    LATCbits.LATC2 = 0;
}

void FAN_SetPowerA(BOOL enabled)
{
    LATCbits.LATC1 = (enabled == TRUE) ? 1 : 0;
}

void FAN_SetPowerB(BOOL enabled)
{
    LATCbits.LATC2 = (enabled == TRUE) ? 1 : 0;
}
