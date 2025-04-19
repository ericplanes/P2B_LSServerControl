#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <xc.h>         // include processor files - each processor file is guarded.
#include <pic18f4321.h> // no necessari a MPLAB
#include "Utils.h"

#define EEPROM_FINISHED 1
#define EEPROM_ONGOING 0

BYTE EEPROM_StoreLog(const BYTE *hhmmssDDMMYYYY);
BYTE EEPROM_ReadLog(BYTE section, BYTE *hhmmssDDMMYYYY);

#endif