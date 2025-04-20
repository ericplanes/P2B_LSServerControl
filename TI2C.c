#include "TI2C.h"

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE index = 0;

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Simulates reading a 14-character timestamp from the RTC via I2C.
 * Returns FALSE (ongoing) until all 14 bytes are transferred, then returns TRUE.
 */
BOOL I2C_ReadTimestamp(BYTE *buffer)
{
    static const BYTE fakeTime[TIMESTAMP_SIZE] = {
        '1', '2', '3', '4', '5', '6',                  // 12:34:56
        '0', '1', '0', '1', '2', '0', '2', '5', '\0'}; // 01-01-2025

    if (index < (TIMESTAMP_SIZE - 1))
    {
        buffer[index] = fakeTime[index];
        index++;
        return FALSE;
    }
    else
    {
        index = 0;
        return TRUE;
    }
}
