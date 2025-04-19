#include "TMenu.h"

/* =======================================
 *              CONFIG STRUCT
 * ======================================= */

typedef struct
{
    char initialTime[15];  // hhmmssDDMMYYYY + '\0'
    uint8_t samplingTime;  // in seconds
    uint8_t thresholds[3]; // thresholds A, B, C
} MenuConfig;

static MenuConfig config;

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */
static void set_default_config(void);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void Menu_Init(void)
{
    set_default_config();
}

void Menu_Motor(void)
{
    // Will be implemented later when Java communication is active
}

const uint8_t *Menu_GetTMPThresholds(void)
{
    return config.thresholds;
}

uint8_t Menu_GetSamplingTime(void)
{
    return config.samplingTime;
}

const char *Menu_GetInitialTimeString(void)
{
    return config.initialTime;
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

static void set_default_config(void)
{
    config.thresholds[0] = 25; // A - Low
    config.thresholds[1] = 35; // B - Moderate
    config.thresholds[2] = 50; // C - High
    config.samplingTime = 20;  // Sampling every 30 seconds

    // Default time: 00:00:00 01/01/2025 → "00000001012025"
    const char *defaultTime = "00000001012025";
    for (int i = 0; i < 14; i++)
    {
        config.initialTime[i] = defaultTime[i];
    }
    config.initialTime[14] = '\0';
}
