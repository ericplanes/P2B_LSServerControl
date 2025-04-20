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

void FAN_SetStateA(BOOL on)
{
    LATAbits.LATA4 = (on == TRUE) ? 1 : 0;
}

void FAN_SetStateB(BOOL on)
{
    LATAbits.LATA5 = (on == TRUE) ? 1 : 0;
}
