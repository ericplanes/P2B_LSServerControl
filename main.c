#include <xc.h>
#include <pic18f4321.h>

#include "Utils.h"
#include "TADC.h"
#include "TAD_TIMER.h"
#include "TMenu.h"
#include "TJoystick.h"
#include "TLed.h"
#include "TFan.h"
#include "TPWM.h"
#include "TController.h"
#include "TTest.h"
#include "TI2C.h"
#include "TRAM.h"
#include "TTemperature.h"

#pragma config OSC = INTIO2
#pragma config PBADEN = DIG
#pragma config MCLRE = OFF
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

void main(void);

/* =======================================
 *               MAIN
 * ======================================= */

void __interrupt() RSI_High(void)
{
    if (INTCONbits.TMR0IF == 1)
    {
        Timer0_ISR();
    }
}

void main(void)
{
    // Set internal oscillator to 8 MHz
    OSCCONbits.IRCF = 0b111; // IRCF = 111: 8 MHz
    OSCTUNEbits.PLLEN = 1;   // Enable 4x PLL => 32 MHz
    OSCCONbits.SCS = 0b00;   // Use clock defined by CONFIG (INTIO67)

    // Now initialize subsystems
    ADC_Init();
    EEPROM_Init();
    CTR_Init();
    FAN_Init();
    Joystick_Init();
    LED_Init();
    MENU_Init();
    PWM_Init(); // Sets FAN and LED to OFF
    RAM_Init(); // Resets RAM
    TEMP_Init();
    I2C_Init();

    TiInit();
    SIO_Init();
    I2C_TEST_InitAlarmEverySecond();
    MENU_TEST_SetDefaultConfig();

    while (TRUE)
    {
        ds3231_HAS_ONE_SECOND_PASSED_YET();
        ADC_Motor();
        MENU_Motor();
        CTR_Motor();
        PWM_Motor();
        SIO_MotorTX();
        SIO_PseudoMotorRX();
    }
}
