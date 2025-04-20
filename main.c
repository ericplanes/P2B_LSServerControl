#include <xc.h>
#include <pic18f4321.h>

#include "Utils.h"
#include "TADC.h"
#include "TTimer.h"
#include "TMenu.h"
#include "TJoystick.h"
#include "TLed.h"
#include "TFan.h"
#include "TPWM.h"
#include "TController.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config MCLRE = OFF
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

void main(void);
void __interrupt() RSI_High(void);

/* =======================================
 *      HIGH PRIORITY INTERRUPT
 * ======================================= */

void __interrupt() RSI_High(void)
{
    if (INTCONbits.TMR0IF == 1)
    {
        Timer0_ISR();
    }
}

/* =======================================
 *               MAIN
 * ======================================= */

void main(void)
{
    // Initialize all TADs
    TiInit();
    ADC_Init();
    MENU_Init();
    Joystick_Init();
    LED_Init();
    FAN_Init();
    PWM_Init();
    CTR_Init();

    // Main loop — cooperative multitasking
    while (TRUE)
    {
        ADC_Motor();
        MENU_Motor();
        CTR_Motor();
        PWM_Motor();
    }
}
