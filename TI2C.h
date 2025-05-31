#ifndef TAD_I2C_H
#define TAD_I2C_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"
#include "TAD_SIO.h"

#define I2C_BAUDRATE 0x24 // Adjust for 100kHz at 10MHz Fosc

/* ========== Public API ========== */

void I2C_Init(void);
void I2C_ReadTimestamp(BYTE *timestamp);
void I2C_SetTimestamp(BYTE hour, BYTE min, BYTE sec, BYTE weekday, BYTE day, BYTE month, BYTE year);
void I2C_UpdateTimestamp(BYTE hour, BYTE min);
void ds3231_HAS_ONE_SECOND_PASSED_YET(void);
BOOL ds3231_HAS_ONE_MINUTE_PASSED_YET(void);

// Testing
void I2C_TEST_PrintTimestamp(void);
void I2C_TEST_InitAlarmEverySecond(void);
void I2C_TEST_Wait1S(void);

// Added functions
void I2C_ReadTimestamp(BYTE *hhmmssDDMMYYYY);

#endif
