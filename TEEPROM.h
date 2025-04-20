#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

STATUS EEPROM_StoreLog(const BYTE *hhmmssDDMMYYYY);
STATUS EEPROM_ReadLog(BYTE section, BYTE *hhmmssDDMMYYYY);

#endif