#include "TTemperature.h"
#include "TMenu.h"
#include "TADC.h"

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */

static BYTE compute_temperature_state(WORD value, const WORD *thresholds);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Returns the current room temperature state based on thresholds A, B, C.
 */
BYTE TEMP_GetState(void)
{
    WORD temp = ADC_GetTemp();
    const WORD *thresholds = MENU_GetTMPThresholds();
    return compute_temperature_state(temp, thresholds);
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

/**
 * Classifies the temperature value into one of the four defined states.
 */
static BYTE compute_temperature_state(WORD value, const WORD *thresholds)
{
    if (value < thresholds[0])
    {
        return TEMP_LOW;
    }
    if (value < thresholds[1])
    {
        return TEMP_MOD;
    }
    if (value < thresholds[2])
    {
        return TEMP_HIGH;
    }
    return TEMP_CRIT;
}
