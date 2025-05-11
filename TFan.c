#include "TFan.h"
#include <xc.h>

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

void FAN_Init(void)
{
    TRISAbits.TRISA4 = 0; // FAN A
    TRISAbits.TRISA5 = 0; // FAN B

    LATAbits.LATA4 = 0;
    LATAbits.LATA5 = 0;
}

void FAN_SetPowerA(BOOL enabled)
{
    LATAbits.LATA4 = (enabled == TRUE) ? 1 : 0;
}

void FAN_SetPowerB(BOOL enabled)
{
    LATAbits.LATA5 = (enabled == TRUE) ? 1 : 0;
}
