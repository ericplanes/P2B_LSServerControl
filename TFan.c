#include "TFan.h"

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

void FAN_Init(void)
{
    TRISCbits.TRISC1 = 0; // FAN A
    TRISCbits.TRISC2 = 0; // FAN B

    FAN_SetPowerA(FALSE);
    FAN_SetPowerB(FALSE);
}

void FAN_SetPowerA(BOOL enabled)
{
    LATCbits.LATC1 = (enabled == TRUE) ? 1 : 0;
}

void FAN_SetPowerB(BOOL enabled)
{
    LATCbits.LATC2 = (enabled == TRUE) ? 1 : 0;
}
