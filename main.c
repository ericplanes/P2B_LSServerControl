#include <stdio.h>
#include <stdlib.h>
#include "C:\Program Files\Microchip\xc8\v3.00\pic\include\xc.h"
#include "C:\Program Files\Microchip\xc8\v3.00\pic\include\proc\pic18f4321.h"
#include "TAD_I2C.h"

#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config PBADEN = DIG

int main(int argc, char **argv)
{
    SIO_Init();
    InitI2C();
    TRISAbits.TRISA4 = 0; // Set RA0 as output
    TRISAbits.TRISA3 = 0; // Set RA0 as output
    TRISDbits.TRISD7 = 1; // Set RD7 as input
    LATAbits.LATA3 = 1;
    DS3231_InitAlarm1_EverySecond();

    while (1)
    {
        if (!PORTDbits.RD7)
        {
            LATAbits.LATA4 ^= 1; // Set RA4 high
            SIO_PrintString("ON");
            SIO_SafePrint('\r');
            SIO_SafePrint('\n');
            DS3231_LimpiarFlagAlarma1();
        }
    }
    return (EXIT_SUCCESS);
}
