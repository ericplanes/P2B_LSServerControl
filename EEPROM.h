#ifndef _EEPROM_H_
#define	_EEPROM_H_

#include <xc.h> // include processor files - each processor file is guarded.
#include <pic18f4321.h> // no necessari a MPLAB

void EEPROM_Save(unsigned char posicioEEPROM, unsigned char dadaEEPROM);
void EEPROM_SetMaxSeccions(unsigned char max);

unsigned char EEPROM_Read(unsigned char posicioEEPROM);
unsigned char EEPROM_SaveTrack(char * nnmmxss);
unsigned char EEPROM_ReadTrack(unsigned char seccio, char * nnmmxss);
unsigned char EEPROM_GetSeccions();

#endif