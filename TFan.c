#include "TFan.h"
#include <xc.h>

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

void FAN_Init(void)
{
    TRISAbits.TRISA4 = 0; // FAN A
    TRISAbits.TRISA5 = 0; // FAN B

    LATAbits.LATA4 = 0; // FAN A OFF
    LATAbits.LATA5 = 0; // FAN B OFF
}

void FAN_SetStateA(BYTE state)
{
    switch (state)
    {
    case FAN_OFF:
        LATAbits.LATA4 = 0;
        break;
    case FAN_ON:
        LATAbits.LATA4 = 1;
        break;
    }
}

void FAN_SetStateB(BYTE state)
{
    switch (state)
    {
    case FAN_OFF:
        LATAbits.LATA5 = 0;
        break;
    case FAN_ON:
        LATAbits.LATA5 = 1;
        break;
    }
}
