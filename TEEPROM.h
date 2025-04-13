#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <xc.h>         // include processor files - each processor file is guarded.
#include <pic18f4321.h> // no necessari a MPLAB

#define EEPROM_FINISHED 1
#define EEPROM_ONGOING 0

unsigned char EEPROM_store_log(char *hhmmssDDMMYYYY);
unsigned char EEPROM_read_log(unsigned char section, char *hhmmssDDMMYYYY);

#endif