#include <stdio.h>
#include <stdlib.h>
#include "C:\Program Files\Microchip\xc8\v3.00\pic\include\xc.h"
#include "C:\Program Files\Microchip\xc8\v3.00\pic\include\proc\pic18f4321.h"
#include "TAD_I2C.h"
#include "TRAM.h"

#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config PBADEN = DIG

int main(int argc, char **argv)
{
    SIO_Init();
    //InitI2C();
    RAM_Init();
    TRISAbits.RA3 = 0; // 
    TRISAbits.RA4 = 0; // 
    TRISAbits.RA5 = 0; // 
    // Establece fecha: 12:56:00 del 7 de mayo de 2025 (miércoles)
    //DS3231_SetFechaHora(00, 56, 12, 3, 10, 5, 25);
    RAM_Reset(); // Escribim dades a la RAM
    RAM_WriteIncremental();
    RAM_ReadAndPrint100();
    while (1)
    {


    }
    return (EXIT_SUCCESS);
}
