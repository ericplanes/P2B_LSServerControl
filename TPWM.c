#include "TPWM.h"
#include "TTimer.h"
#include "TLed.h"
#include "TFan.h"
#include "TController.h"

/* =======================================
 *           PRIVATE CONSTANTS
 * ======================================= */

#define FAN_PERIOD_MS 20        // Total PWM cycle = 20ms
#define FAN_DUTY_ON_MS 10       // ON time = 10ms (50%)
#define CRIT_LED_TOGGLE_MS 1000 // Toggle LED every 1s

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE timerFAN;
static BYTE timerLED;

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void PWM_Init(void)
{
    timerFAN = TiGetTimer();
    TiResetTics(timerFAN);

    timerLED = TiGetTimer();
    TiResetTics(timerLED);

    FAN_SetPowerA(FALSE);
    FAN_SetPowerB(FALSE);
    LED_SetColor(LED_OFF);
}

void PWM_Motor(void)
{
    SYS_STATUS status = CTR_GetStatus();
    WORD ticsFAN = TiGetTics(timerFAN);
    WORD ticsLED = TiGetTics(timerLED);

    switch (status)
    {
    case SYS_STATUS_LOW:
        FAN_SetPowerA(ticsFAN < FAN_DUTY_ON_MS ? TRUE : FALSE);
        FAN_SetPowerB(FALSE);
        LED_SetColor(LED_GREEN);
        break;

    case SYS_STATUS_MOD:
    {
        BOOL pwm_on = (ticsFAN < FAN_DUTY_ON_MS);
        FAN_SetPowerA(pwm_on);
        FAN_SetPowerB(pwm_on);
        LED_SetColor(LED_BLUE);
    }
    break;

    case SYS_STATUS_HIGH:
        FAN_SetPowerA(TRUE);
        FAN_SetPowerB(TRUE);
        LED_SetColor(LED_RED);
        break;

    case SYS_STATUS_CRIT:
        FAN_SetPowerA(FALSE);
        FAN_SetPowerB(FALSE);

        if (ticsLED < CRIT_LED_TOGGLE_MS)
            LED_SetColor(LED_RED);
        else
            LED_SetColor(LED_MAGENTA);

        if (ticsLED >= 2 * CRIT_LED_TOGGLE_MS)
            TiResetTics(timerLED);
        break;

    case SYS_STATUS_OFF:
        FAN_SetPowerA(FALSE);
        FAN_SetPowerB(FALSE);
        LED_SetColor(LED_OFF);
        TiResetTics(timerFAN);
        TiResetTics(timerLED);
        break;
    }

    if (ticsFAN >= FAN_PERIOD_MS)
        TiResetTics(timerFAN);
}
