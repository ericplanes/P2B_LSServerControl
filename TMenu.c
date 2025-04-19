// Menu is not yet implemented, but since it will read the configuration that is needed, I'll add it here
#include "TMenu.h"

/* =======================================
 *              CONFIG STRUCT
 * ======================================= */

typedef struct
{
    char initialTime[15];  // hhmmssDDMMYYYY + '\0'
    uint8_t samplingTime;  // en segons
    uint8_t thresholds[3]; // thresholds A, B, C
} MenuConfig;

static MenuConfig config;

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */
void set_default_config(void);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Initializes menu configuration with default values.
 * These would normally be overwritten by the Java interface.
 */
void Menu_Init(void)
{
    set_default_config();
}

void Menu_Motor(void)
{
}

/**
 * Returns a pointer to the temperature threshold array.
 * The array contains:
 * [0] = A (Low threshold)
 * [1] = B (Moderate threshold)
 * [2] = C (High threshold)
 */
const uint8_t *Menu_GetThresholds(void)
{
    return config.thresholds;
}

uint8_t Menu_GetSamplingTime(void)
{
    return config.samplingTime
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

void set_default_config(void)
{
    config.thresholds[0] = 25; // A - Low
    config.thresholds[1] = 30; // B - Moderate
    config.thresholds[2] = 35; // C - High
    config.samplingTime = 30   // Sampling time 30 seconds
}