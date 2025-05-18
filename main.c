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
#include "TTemperature.h"

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
    FAN_Init(); // Check
    Joystick_Init();
    LED_Init();  // Check
    MENU_Init(); // Check
    PWM_Init();
    RAM_Init(); // Check
    TEMP_Init();

    // TiInit();

    // Test and inits I2C
    SIO_Init();
    testLectura();

    // Extras for testing
    RAM_Reset();
    DS3231_InitAlarm1_EverySecond();

    // Main loop — cooperative multitasking
    int i = 1;
    while (TRUE)
    {
        ADC_Motor();
        MENU_Motor();
        CTR_Motor();
        PWM_Motor();

        // Every 100 iterations, write actual state
        if (i == 2000)
        {
            i = 0;
            TEST_print_results();
        }
        i++;
    }
}
