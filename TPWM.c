#include "TPWM.h"
#include "TAD_TIMER.h"
#include "TLed.h"
#include "TFan.h"
#include "TController.h"

/* =======================================
 *           PRIVATE CONSTANTS
 * ======================================= */

#define PWM_PERIOD_MS 20        // Total PWM cycle = 20ms
#define PWM_DUTY_ON_MS 5        // ON time = 5ms (25%)
#define CRIT_LED_TOGGLE_MS 1000 // Toggle LED every 1s

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE timerFan = TI_FAN;
static BYTE timerLED = TI_LED;

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void PWM_Init(void)
{
    TiNewTimer(&timerFan);
    TiResetTics(timerFan);

    TiNewTimer(&timerLED);
    TiResetTics(timerLED);

    FAN_SetPowerA(FALSE);
    FAN_SetPowerB(FALSE);
    LED_SetColor(LED_OFF);
}

void PWM_Motor(void)
{
    SYS_STATUS status = CTR_GetStatus();
    WORD ticsPWM = TiGetTics(timerFan);
    WORD ticsLED = TiGetTics(timerLED);

    switch (status)
    {
    case SYS_STATUS_LOW:
        FAN_SetPowerA(ticsPWM < PWM_DUTY_ON_MS ? TRUE : FALSE);
        FAN_SetPowerB(FALSE);
        LED_SetColor(LED_GREEN);
        break;

    case SYS_STATUS_MOD:
    {
        BOOL pwm_on = (ticsPWM < PWM_DUTY_ON_MS);
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
        TiResetTics(timerFan);
        TiResetTics(timerLED);
        break;
    }

    if (ticsPWM >= PWM_PERIOD_MS)
        TiResetTics(timerFan);
}
