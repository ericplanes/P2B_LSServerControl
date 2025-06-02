#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

void EEPROM_Init(void);
BOOL EEPROM_StoreLog(const BYTE *hhmmssDDMMYYYY);
BOOL EEPROM_ReadLog(BYTE section, BYTE *hhmmssDDMMYYYY);
BYTE EEPROM_GetAmountOfStoredLogs(void);
void EEPROM_CleanMemory(void);
BYTE EEPROM_GetFirstSection(void);
BYTE EEPROM_GetNextSection(BYTE previous_section);

#endif
