#include "TLED.h"
#include <xc.h>

void LED_Init(void)
{
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
    TRISDbits.TRISD7 = 0;

    LED_SetColor(LED_OFF);
}

void LED_SetColor(BYTE color)
{
    switch (color)
    {
    case LED_RED:
        LATDbits.LATD5 = 1;
        LATDbits.LATD6 = 0;
        LATDbits.LATD7 = 0;
        break;

    case LED_GREEN:
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 1;
        LATDbits.LATD7 = 0;
        break;

    case LED_BLUE:
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 0;
        LATDbits.LATD7 = 1;
        break;

    case LED_MAGENTA:
        LATDbits.LATD5 = 1;
        LATDbits.LATD6 = 0;
        LATDbits.LATD7 = 1;
        break;

    case LED_OFF:
    default:
        LATDbits.LATD5 = 0;
        LATDbits.LATD6 = 0;
        LATDbits.LATD7 = 0;
        break;
    }
}
