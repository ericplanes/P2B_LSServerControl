#include "TLed.h"

static BOOL toggle_state = FALSE;

void LED_Init(void)
{
    TRISAbits.TRISA3 = 0; // Red
    TRISAbits.TRISA5 = 0; // Green
    TRISAbits.TRISA4 = 0; // Blue
    LED_SetColor(LED_OFF);
}

void LED_SetColor(BYTE color)
{
    switch (color)
    {
    case LED_RED:
        LATAbits.LATA3 = 1;
        LATAbits.LATA5 = 0;
        LATAbits.LATA4 = 0;
        break;

    case LED_GREEN:
        LATAbits.LATA3 = 0;
        LATAbits.LATA5 = 1;
        LATAbits.LATA4 = 0;
        break;

    case LED_BLUE:
        LATAbits.LATA3 = 0;
        LATAbits.LATA5 = 0;
        LATAbits.LATA4 = 1;
        break;

    case LED_MAGENTA:
        LATAbits.LATA3 = 1;
        LATAbits.LATA5 = 0;
        LATAbits.LATA4 = 1;
        break;

    case LED_OFF:
    default:
        LATAbits.LATA3 = 0;
        LATAbits.LATA5 = 0;
        LATAbits.LATA4 = 0;
        break;
    }
}
