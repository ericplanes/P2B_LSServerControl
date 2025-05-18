#include "TTemperature.h"
#include "TMenu.h"
#include "TADC.h"

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */

static SYS_STATUS compute_temperature_state(BYTE value, const BYTE *thresholds);
static BYTE compute_temperature_degrees(WORD adc);

/* =======================================
 *          PRIVATE VARIABLES
 * ======================================= */

static BYTE temperature;
static BOOL simulation;
static SYS_STATUS fake;

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void TEMP_Init(void)
{
    temperature = 0;
    simulation = FALSE;
    fake = SYS_STATUS_OFF;
}

/**
 * Returns the current room temperature state based on thresholds A, B, C.
 */
SYS_STATUS TEMP_GetState(void)
{
    WORD adc = ADC_GetTemp();
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

/**
 * TESTING METHOD -> Forces a state of the temperature to tweak the system
 */
void TEMP_TEST_SimulateState(SYS_STATUS state)
{
    const BYTE *thresholds = MENU_GetTMPThresholds();
    if (state == SYS_STATUS_OFF)
        TEMP_Init();

    simulation = TRUE;
    fake = state;
    temperature = state * (11 - state); // 0 (OFF), 10 (LOW), 18 (MID), 24 (HIGH), 28 (CRIT)
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

/*
 * Computes the value of the temperature from the value readed at the ADC (XXXXXX-HH LLLLLLLL).
 * Function used -> TMP36: Vout = 0.5 + 10mV/°C → 0.5V = 0 °C
 * Each step ADC ≈ 4.88 mV → 1 step = 0.488°C
 */
static BYTE compute_temperature_degrees(WORD adc)
{
    if (simulation == TRUE)
        return temperature;

    return 20; // Provisional number until the conversion works correctly
}

/**
 * Classifies the temperature value into one of the four defined states.
 */
static SYS_STATUS compute_temperature_state(BYTE value, const BYTE *thresholds)
{
    if (simulation == TRUE)
        return fake;

    if (value < thresholds[0])
        return SYS_STATUS_LOW;

    if (value < thresholds[1])
        return SYS_STATUS_MOD;

    if (value < thresholds[2])
        return SYS_STATUS_HIGH;

    return SYS_STATUS_CRIT;
}
