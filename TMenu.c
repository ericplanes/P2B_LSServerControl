#include "TMenu.h"

/* =======================================
 *              CONFIG STRUCT
 * ======================================= */

typedef struct
{
    BYTE initialTime[15]; // hhmmssDDMMYYYY + '\0'
    BYTE samplingTime;    // in seconds
    WORD thresholds[3];   // thresholds A, B, C
    BYTE isConfigured;
} MenuConfig;

static MenuConfig config;

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */
static void set_default_config(void);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void MENU_Init(void)
{
    set_default_config();
}

void MENU_Motor(void)
{
    // Will be implemented later when Java communication is active
}

BYTE MENU_isConfigured(void)
{
    return config.isConfigured;
}

const WORD *MENU_GetTMPThresholds(void)
{
    return config.thresholds;
}

BYTE MENU_GetSamplingTime(void)
{
    return config.samplingTime;
}

const BYTE *MENU_GetInitialTimeString(void)
{
    return config.initialTime;
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

static void set_default_config(void)
{
    config.isConfigured = FALSE; // Since we're using the default config

    config.thresholds[0] = 25; // A - Low
    config.thresholds[1] = 35; // B - Moderate
    config.thresholds[2] = 50; // C - High
    config.samplingTime = 20;  // Sampling every 30 seconds

    // Default time: 00:00:00 01/01/2025 → "00000001012025"
    const BYTE *defaultTime = (const BYTE *)"00000001012025";
    for (int i = 0; i < 14; i++)
    {
        config.initialTime[i] = defaultTime[i];
    }
    config.initialTime[14] = '\0';
}
