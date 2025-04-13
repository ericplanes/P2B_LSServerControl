#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <xc.h>         // include processor files - each processor file is guarded.
#include <pic18f4321.h> // no necessari a MPLAB

#define EEPROM_FINISHED 1
#define EEPROM_ONGOING 0

unsigned char EEPROM_StoreLog(char *hhmmssDDMMYYYY);
unsigned char EEPROM_ReadLog(unsigned char seccio, char *nnmmxss);
unsigned char EEPROM_ResetLogs();

#endif