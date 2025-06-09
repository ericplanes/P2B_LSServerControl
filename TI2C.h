#ifndef TAD_I2C_H
#define TAD_I2C_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

#define I2C_BAUDRATE 0x24 // Adjust for 100kHz at 10MHz Fosc

/* ========== Public API ========== */

void I2C_Init(void);
void I2C_ReadTimestamp(BYTE *timestamp);
void I2C_SetTimestamp(BYTE hour, BYTE min, BYTE sec, BYTE weekday, BYTE day, BYTE month, BYTE year);
void I2C_UpdateTimestamp(BYTE hour, BYTE min);
void I2C_CheckCounter1S(void);
BOOL I2C_OneMinutePassed(void);

#endif
