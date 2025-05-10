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
    TRISAbits.TRISA5 = 0; // green
    TRISAbits.TRISA4 = 0; // blue
    TRISAbits.TRISA3 = 0; // red
    TRISD = 0XFF; // Set RD7 as input
    int i = 0;
    DS3231_InitAlarm1_EverySecond();

    unsigned char seg, min, hora, dia_sem, dia, mes, anio;
    char buffer[6];

    // Establece fecha: 12:56:00 del 7 de mayo de 2025 (miércoles)
    //DS3231_SetFechaHora(00, 56, 12, 3, 10, 5, 25);
    
    while (1)
    {
        if (!PORTBbits.RB1)
        {
            /* //TEST escriu lo just
            SIO_PrintString("ON");
            SIO_SafePrint('\r');
            SIO_SafePrint('\n');
            */
            DS3231_LimpiarFlagAlarma1();
            
            DS3231_LeerFechaHora(&seg, &min, &hora, &dia_sem, &dia, &mes, &anio);
            SIO_PrintString("\r\n");
            // Imprimir fecha en formato DD/MM/AA
            itoa(dia, buffer, 10);        SIO_PrintString(buffer); SIO_SafePrint('/');
            itoa(mes, buffer, 10);        SIO_PrintString(buffer); SIO_SafePrint('/');
            itoa(anio, buffer, 10);       SIO_PrintString(buffer); SIO_PrintString(" ");

            // Imprimir hora en formato HH:MM:SS
            itoa(hora, buffer, 10);       SIO_PrintString(buffer); SIO_SafePrint(':');
            itoa(min, buffer, 10);        SIO_PrintString(buffer); SIO_SafePrint(':');
            itoa(seg, buffer, 10);        SIO_PrintString(buffer); SIO_PrintString("\r\n");

            LATAbits.LA3 = 0;
            LATAbits.LA4 = 0;
            LATAbits.LA5 = 0;
            switch (i){
                case 0:
                    LATAbits.LA3 ^= 1;
                    i++;
                    break;
                case 1:
                    LATAbits.LA4 ^= 1;
                    i++;
                    break;
                case 2:
                    LATAbits.LA5 ^= 1;
                    i=0;
                    break;
                    default:
                    break;
            }
        }
    }
    return (EXIT_SUCCESS);
}
