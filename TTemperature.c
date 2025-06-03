#include "TTemperature.h"

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */

static SYS_STATUS compute_temperature_state(BYTE value, const BYTE *thresholds);
static BYTE compute_temperature_degrees(BYTE adc);

/* =======================================
 *          PRIVATE VARIABLES
 * ======================================= */

static BYTE temperature;
static SYS_STATUS fake;

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void TEMP_Init(void)
{
    temperature = 0;
    fake = SYS_STATUS_OFF;
}

/**
 * Returns the current room temperature state based on thresholds A, B, C.
 */
SYS_STATUS TEMP_GetState(void)
{
    BYTE adc = ADC_GetTemp();
    temperature = compute_temperature_degrees(adc);
    const BYTE *thresholds = MENU_GetTMPThresholds();
    return compute_temperature_state(temperature, thresholds);
}

/**
 * Returns the last-read temperature degrees
 */
BYTE TEMP_GetTemperature(void)
{
    return temperature;
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

/*
 * Computes the value of the temperature from the value readed at the ADC
 */
static BYTE compute_temperature_degrees(BYTE adc)
{
    return (adc * 6) / 10;
}

/**
 * Classifies the temperature value into one of the four defined states.
 */
static SYS_STATUS compute_temperature_state(BYTE value, const BYTE *thresholds)
{
    if (value < thresholds[0])
        return SYS_STATUS_LOW;

    if (value < thresholds[1])
        return SYS_STATUS_MOD;

    if (value < thresholds[2])
        return SYS_STATUS_HIGH;

    return SYS_STATUS_CRIT;
}
