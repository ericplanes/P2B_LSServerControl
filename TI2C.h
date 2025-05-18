#ifndef TAD_I2C_H
#define TAD_I2C_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"
#include "TAD_SIO.h"
#include "Utils.h"

#define I2C_BAUDRATE 0x24 // Adjust for 100kHz at 10MHz Fosc

/* ========== Public API ========== */

void I2C_Init(void);
void I2C_ReadTimestamp(BYTE *timestamp);
void I2C_SetTimestamp(BYTE hour, BYTE min, BYTE sec, BYTE weekday, BYTE day, BYTE month, BYTE year);

// Testing
void I2C_TEST_PrintTimestamp(void);
void I2C_TEST_InitAlarmEverySecond(void);
void I2C_TEST_Wait1S(void);

// Added functions
void I2C_ReadTimestamp(BYTE *hhmmssDDMMYYYY);
void testLectura(void);

#endif
