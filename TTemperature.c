#include "TTemperature.h"
#include "TMenu.h"
#include "TADC.h"

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */

static uint8_t compute_temperature_state(uint8_t value, const uint8_t *thresholds);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Returns the current room temperature state based on thresholds A, B, C.
 */
uint8_t Temperature_GetState(void)
{
    uint8_t temp = ADC_GetTemp();
    const uint8_t *thresholds = Menu_GetTMPThresholds();
    return compute_temperature_state(temp, thresholds);
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

/**
 * Classifies the temperature value into one of the four defined states.
 */
static uint8_t compute_temperature_state(uint8_t value, const uint8_t *thresholds)
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
