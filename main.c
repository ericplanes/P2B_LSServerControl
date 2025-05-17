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
#include "TTest.h"
#include "TI2C.h"
#include "TRAM.h"

#pragma config OSC = HS
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

void main(void)
{
    // Initialize all TADs
    ADC_Init();
    CTR_Init();
    FAN_Init();
    Joystick_Init();
    LED_Init();
    MENU_Init();
    PWM_Init();
    RAM_Init();
    TiInit();

    // Test and inits I2C
    testLectura();

    // Extras for testing
    RAM_Reset();
    SIO_Init();
    DS3231_InitAlarm1_EverySecond();

    // Main loop — cooperative multitasking
    while (TRUE)
    {
        ADC_Motor();
        MENU_Motor();
        CTR_Motor();
        PWM_Motor();
        TEST_print_results();
    }
}
