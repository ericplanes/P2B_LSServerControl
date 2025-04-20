#include "TPWM.h"
#include "TTimer.h"
#include "TLed.h"
#include "TFan.h"
#include "TController.h"

/* =======================================
 *           PRIVATE CONSTANTS
 * ======================================= */

#define PWM_PERIOD_MS 20        // Total PWM cycle = 20ms
#define PWM_DUTY_ON_MS 10       // ON time = 10ms (50%)
#define CRIT_LED_TOGGLE_MS 1000 // Toggle LED every 1s

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE timerPWM;
static BYTE timerLED;

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void PWM_Init(void)
{
    timerPWM = TiGetTimer();
    TiResetTics(timerPWM);

    timerLED = TiGetTimer();
    TiResetTics(timerLED);

    FAN_SetStateA(FAN_OFF);
    FAN_SetStateB(FAN_OFF);
    LED_SetColor(LED_OFF);
}

void PWM_Motor(void)
{
    STATUS status = CTR_GetStatus();
    WORD ticsPWM = TiGetTics(timerPWM);
    WORD ticsLED = TiGetTics(timerLED);

    switch (status)
    {
    case STATUS_LOW:
        if (ticsPWM < PWM_DUTY_ON_MS)
            FAN_SetStateA(FAN_ON);
        else
            FAN_SetStateA(FAN_OFF);

        FAN_SetStateB(FAN_OFF);
        LED_SetColor(LED_GREEN);
        break;

    case STATUS_MOD:
        if (ticsPWM < PWM_DUTY_ON_MS)
        {
            FAN_SetStateA(FAN_ON);
            FAN_SetStateB(FAN_ON);
        }
        else
        {
            FAN_SetStateA(FAN_OFF);
            FAN_SetStateB(FAN_OFF);
        }
        LED_SetColor(LED_BLUE);
        break;

    case STATUS_HIGH:
        FAN_SetStateA(FAN_ON);
        FAN_SetStateB(FAN_ON);
        LED_SetColor(LED_RED);
        break;

    case STATUS_CRIT:
        FAN_SetStateA(FAN_OFF);
        FAN_SetStateB(FAN_OFF);

        if (ticsLED < CRIT_LED_TOGGLE_MS)
            LED_SetColor(LED_RED);
        else
            LED_SetColor(LED_MAGENTA);

        if (ticsLED >= 2 * CRIT_LED_TOGGLE_MS)
            TiResetTics(timerLED);
        break;

    case STATUS_OFF:
        FAN_SetStateA(FAN_OFF);
        FAN_SetStateB(FAN_OFF);
        LED_SetColor(LED_OFF);
        TiResetTics(timerPWM);
        TiResetTics(timerLED);
        break;
    }

    if (ticsPWM >= PWM_PERIOD_MS)
        TiResetTics(timerPWM);
}
