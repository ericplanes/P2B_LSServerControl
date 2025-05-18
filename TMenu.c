#include "TMenu.h"

/* =======================================
 *              CONFIG STRUCT
 * ======================================= */

typedef struct
{
    BYTE initialTime[15]; // hhmmssDDMMYYYY + '\0'
    BYTE samplingTime;    // in seconds
    BYTE thresholds[3];   // thresholds A, B, C
    BYTE isConfigured;
} MenuConfig;

static MenuConfig config;

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void MENU_Init(void)
{
}

void MENU_Motor(void)
{
    // Will be implemented later when Java communication is active
}

BOOL MENU_isConfigured(void)
{
    return config.isConfigured == TRUE;
}

const BYTE *MENU_GetTMPThresholds(void)
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

void MENU_TEST_SetDefaultConfig(void)
{
    config.isConfigured = TRUE;

    config.thresholds[0] = 20; // A - Low Temperature
    config.thresholds[1] = 25; // B - Moderate Temperature
    config.thresholds[2] = 30; // C - High Temperature
    config.samplingTime = 1;   // Sampling every 1 seconds

    // Default time: 00:00:00 01/01/2025 → "00000001012025"
    const BYTE *defaultTime = (const BYTE *)"00000001012025";
    for (int i = 0; i < 14; i++)
    {
        config.initialTime[i] = defaultTime[i];
    }
    config.initialTime[14] = '\0';
}
