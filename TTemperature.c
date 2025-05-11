#include "TTemperature.h"
#include "TMenu.h"
#include "TADC.h"

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */

static SYS_STATUS compute_temperature_state(WORD value, const WORD *thresholds);

/* =======================================
 *          PRIVATE VARIABLES
 * ======================================= */

static WORD temperature;

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Returns the current room temperature state based on thresholds A, B, C.
 */
SYS_STATUS TEMP_GetState(void)
{
    temperature = ADC_GetTemp();
    const WORD *thresholds = MENU_GetTMPThresholds();
    return compute_temperature_state(temperature, thresholds);
}

WORD TEMP_GetTemperature(void)
{
    return temperature;
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

/**
 * Classifies the temperature value into one of the four defined states.
 */
static SYS_STATUS compute_temperature_state(WORD value, const WORD *thresholds)
{
    if (value < thresholds[0])
        return SYS_STATUS_LOW;

    if (value < thresholds[1])
        return SYS_STATUS_MOD;

    if (value < thresholds[2])
        return SYS_STATUS_HIGH;

    return SYS_STATUS_CRIT;
}
