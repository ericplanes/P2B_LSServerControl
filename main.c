#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"
#include "TADC.h"
#include "TTimer.h"
#include "TMenu.h"
#include "TJoystick.h"

#pragma config OSC = HSPLL // PLL because freq is 40MH
#pragma config PBADEN = DIG
#pragma config MCLRE = OFF
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

void main(void);
void __interrupt() RSI_High(void);

void __interrupt() RSI_High(void)
{
    if (INTCONbits.TMR0IF == 1)
    {
        // Codi que va amb interrupcions
    }
}

void main()
{
    // Here we'll init all the tads that we need to init
    TiInit();
    ADC_Init();
    Menu_Init();
    Joystick_Init();

    // Here we'll put the motors, so that they're cooperatively called
    while (TRUE)
    {
        ADC_Motor();
        Menu_Motor();
    }
}
