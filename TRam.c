#include "TRam.h"

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE writeStage = 0;

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Simulates storing a WORD (2 bytes) into RAM byte by byte.
 * Returns FALSE (ongoing) the first time and TRUE (done) after the second byte.
 */
BOOL RAM_StoreTemperature(WORD temperature)
{
    switch (writeStage)
    {
    case 0:
        // Simulate writing LSB
        // Ex: write_byte(address, (BYTE)(temperature & 0xFF));
        writeStage++;
        return FALSE;

    case 1:
        // Simulate writing MSB
        // Ex: write_byte(address + 1, (BYTE)(temperature >> 8));
        writeStage = 0;
        return TRUE;

    default:
        writeStage = 0;
        return FALSE;
    }
}
