#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

BOOL EEPROM_StoreLog(const BYTE *hhmmssDDMMYYYY);
BOOL EEPROM_ReadLog(BYTE section, BYTE *hhmmssDDMMYYYY);
BYTE EEPROM_GetAmountOfStoredLogs(void);
BYTE EEPROM_CanBeUsed(void);

#endif
